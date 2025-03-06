#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QSettings>
#include <QStyle>
#include <QApplication>
#include <QColor>
#include "settingsdialog.h"
#include "taskprogressbar.h"
#include "task.h"
#include <QVector>
#include <QInputDialog>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <algorithm>
#include <QListWidgetItem>
#include <QIcon>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include "taskeditdialog.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , remainingTime(25 * 60)
    , isWorkTime(true)
    , isRunning(false)
    , workTime(25 * 60)   // 默认25分钟
    , breakTime(5 * 60)   // 默认5分钟
    , totalCycles(4)      // 默认4个循环
    , currentCycle(1)     // 从第1个循环开始
    , currentTaskIndex(-1)
{
    ui->setupUi(this);
    
    // 创建圆形进度条
    circularProgress = new CircularProgress(this);
    ui->focusLayout->insertWidget(0, circularProgress);
    
    // 创建任务进度条
    taskProgress = new TaskProgressBar(this);
    ui->progressLayout->insertWidget(0, taskProgress);  // 插入到状态标签上面
    
    // 设置颜色
    if (isWorkTime) {
        circularProgress->setColor(QColor(76, 175, 80));  // 工作时间用绿色
    } else {
        circularProgress->setColor(QColor(33, 150, 243));  // 休息时间用蓝色
    }
    
    // 初始化系统托盘图标
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    trayIcon->show();
    
    // 初始化定时器
    timer = new QTimer(this);
    timer->setInterval(1000); // 1秒
    
    // 初始化音效播放器
    workSound = new QMediaPlayer(this);
    breakSound = new QMediaPlayer(this);
    completeSound = new QMediaPlayer(this);
    workAudio = new QAudioOutput(this);
    breakAudio = new QAudioOutput(this);
    completeAudio = new QAudioOutput(this);
    workSound->setAudioOutput(workAudio);
    breakSound->setAudioOutput(breakAudio);
    completeSound->setAudioOutput(completeAudio);
    workAudio->setVolume(1.0);
    breakAudio->setVolume(1.0);
    completeAudio->setVolume(1.0);
    
    // 连接信号和槽
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartButtonClicked);
    connect(ui->pauseButton, &QPushButton::clicked, this, &MainWindow::onPauseButtonClicked);
    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::onResetButtonClicked);
    
    // 连接任务管理相关信号
    connect(ui->addTaskButton, &QPushButton::clicked, this, &MainWindow::addNewTask);
    connect(ui->editTaskButton, &QPushButton::clicked, this, &MainWindow::editTask);
    connect(ui->deleteTaskButton, &QPushButton::clicked, this, &MainWindow::deleteTask);
    connect(ui->taskComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onTaskSelected);
    connect(ui->taskList, &QListWidget::currentRowChanged,
            this, &MainWindow::onTaskSelected);
    
    // 连接音效相关按钮信号
    connect(ui->browseWorkSoundButton, &QPushButton::clicked, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(this,
            QString::fromUtf8("选择专注音效文件"),
            QString(),
            QString::fromUtf8("音效文件 (*.mp3 *.wav *.ogg *.m4a)"));
            
        if (!filePath.isEmpty()) {
            workSoundPath = filePath;
            workSound->setSource(QUrl::fromLocalFile(filePath));
            ui->workSoundEdit->setText(filePath);
            saveSettings();
        }
    });
    
    connect(ui->browseBreakSoundButton, &QPushButton::clicked, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(this,
            QString::fromUtf8("选择休息音效文件"),
            QString(),
            QString::fromUtf8("音效文件 (*.mp3 *.wav *.ogg *.m4a)"));
            
        if (!filePath.isEmpty()) {
            breakSoundPath = filePath;
            breakSound->setSource(QUrl::fromLocalFile(filePath));
            ui->breakSoundEdit->setText(filePath);
            
            // 调用 saveSettings() 方法保存路径
            saveSettings();  // 确保调用保存设置
        }
    });
    
    connect(ui->testWorkSoundButton, &QPushButton::clicked, this, [this]() {
        playSound(workSound);
    });
    
    connect(ui->testBreakSoundButton, &QPushButton::clicked, this, [this]() {
        playSound(breakSound);
    });
    
    connect(ui->enableWorkSoundCheck, &QCheckBox::toggled, this, [this](bool checked) {
        enableWorkSound = checked;  // 更新专注音效启用状态
        saveSettings();  // 保存设置
    });
    
    connect(ui->enableBreakSoundCheck, &QCheckBox::toggled, this, [this](bool checked) {
        enableBreakSound = checked;  // 更新休息音效启用状态
        saveSettings();  // 保存设置
    });
    
    // 添加完成音效相关按钮信号
    connect(ui->browseCompleteSoundButton, &QPushButton::clicked, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(this,
            QString::fromUtf8("选择完成音效文件"),
            QString(),
            QString::fromUtf8("音效文件 (*.mp3 *.wav *.ogg *.m4a)"));
            
        if (!filePath.isEmpty()) {
            completeSoundPath = filePath;
            completeSound->setSource(QUrl::fromLocalFile(filePath));
            ui->completeSoundEdit->setText(filePath);
            saveSettings();
        }
    });
    
    connect(ui->testCompleteSoundButton, &QPushButton::clicked, this, [this]() {
        playSound(completeSound);
    });
    
    connect(ui->enableCompleteSoundCheck, &QCheckBox::toggled, this, [this](bool checked) {
        enableCompleteSound = checked;  // 更新完成音效启用状态
        saveSettings();  // 保存设置
    });
    
    // 连接时间设置控件的信号
    connect(ui->workMinSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::saveSettings);
    connect(ui->breakMinSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::saveSettings);
    connect(ui->cyclesSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::saveSettings);
    
    // 初始化显示
    updateDisplay();
    
    // 初始化设置
    loadSettings();
    
    // 加载任务列表
    loadTasks();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onStartButtonClicked()
{
    isRunning = true;
    timer->start();
    ui->startButton->setEnabled(false);
    ui->pauseButton->setEnabled(true);
    ui->resetButton->setEnabled(false);  // 运行时禁用重置按钮
    
    // 禁用时间设置控件
    ui->workMinSpinBox->setEnabled(false);
    ui->breakMinSpinBox->setEnabled(false);
    ui->cyclesSpinBox->setEnabled(false);
    
    // 禁用任务管理相关控件
    ui->taskList->setEnabled(false);
    ui->taskComboBox->setEnabled(false);
    ui->addTaskButton->setEnabled(false);
    ui->editTaskButton->setEnabled(false);
    ui->deleteTaskButton->setEnabled(false);
    
    // 禁用设置相关控件
    ui->tabWidget->setTabEnabled(1, false);  // 禁用任务管理标签页
    ui->tabWidget->setTabEnabled(2, false);  // 禁用时间统计标签页
    ui->tabWidget->setTabEnabled(3, false);  // 禁用设置标签页
}

void MainWindow::onPauseButtonClicked()
{
    isRunning = false;
    timer->stop();
    
    ui->startButton->setEnabled(true);
    ui->startButton->setText("继续");  // 暂停后显示"继续"
    ui->pauseButton->setEnabled(false);
    ui->resetButton->setEnabled(true);   // 暂停时启用重置按钮
    
    // 启用时间设置控件
    ui->workMinSpinBox->setEnabled(true);
    ui->breakMinSpinBox->setEnabled(true);
    ui->cyclesSpinBox->setEnabled(true);
    
    // 启用任务管理相关控件
    ui->taskList->setEnabled(true);
    ui->taskComboBox->setEnabled(true);
    ui->addTaskButton->setEnabled(true);
    ui->editTaskButton->setEnabled(true);
    ui->deleteTaskButton->setEnabled(true);
    
    // 启用设置相关控件
    ui->tabWidget->setTabEnabled(1, true);  // 启用任务管理标签页
    ui->tabWidget->setTabEnabled(2, true);  // 启用时间统计标签页
    ui->tabWidget->setTabEnabled(3, true);  // 启用设置标签页
}

void MainWindow::onResetButtonClicked()
{
    // 只有在暂停状态下才能重置
    if (isRunning) {
        return;
    }

    // 如果是工作时间且有选中的任务，则计入已专注的时间
    if (isWorkTime && currentTaskIndex >= 0) {
        int focusedTime = workTime - remainingTime;  // 计算这次专注了多少时间
        if (focusedTime > 0) {
            tasks[currentTaskIndex].totalSeconds += focusedTime;  // 增加总时长
            saveTasks();  // 保存任务
            updateStats();  // 更新统计信息
        }
    }

    // 重置为当前模式的初始状态
    isWorkTime = true;
    currentCycle = 1;
    remainingTime = workTime;  // 使用当前设置的工作时间
    
    // 更新UI
    ui->statusLabel->setText(QString("正在专注 (1/%1)").arg(totalCycles));
    ui->startButton->setEnabled(true);
    ui->startButton->setText("开始");  // 重置后显示"开始"
    ui->pauseButton->setEnabled(false);
    
    // 重置进度条颜色为工作时间的颜色
    circularProgress->setColor(QColor(76, 175, 80));
    
    updateDisplay();
}

void MainWindow::updateTimer()
{
    if (remainingTime > 0) {
        remainingTime--;
        updateDisplay();
    } else {
        // 不停止计时器，直接切换模式
        switchMode();
    }
}

void MainWindow::updateDisplay()
{
    int minutes = remainingTime / 60;
    int seconds = remainingTime % 60;
    QString timeStr = QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
    
    // 更新圆形进度条
    int totalTime = isWorkTime ? workTime : breakTime;
    int progress = ((totalTime - remainingTime) * 100) / totalTime;
    circularProgress->setProgress(progress);
    circularProgress->setTimeText(timeStr);
    
    // 更新状态标签
    if (isWorkTime) {
        ui->statusLabel->setText(QString("正在专注 (%1/%2)")
            .arg(currentCycle)
            .arg(totalCycles));
    } else {
        ui->statusLabel->setText(QString("休息时间 (%1/%2)")
            .arg(currentCycle)
            .arg(totalCycles - 1));
    }
    
    // 计算总秒数和当前秒数
    int totalSeconds = (workTime * totalCycles) + (breakTime * (totalCycles - 1));
    int currentSeconds;
    
    if (isWorkTime) {
        currentSeconds = (currentCycle - 1) * (workTime + breakTime) + (workTime - remainingTime);
    } else {
        currentSeconds = currentCycle * workTime + ((currentCycle - 1) * breakTime) + (breakTime - remainingTime);
    }

    // 更新任务进度条
    taskProgress->setTimes(workTime, breakTime);
    taskProgress->updateProgress(currentSeconds, isWorkTime);
}

void MainWindow::switchMode()
{
    if (isWorkTime && currentTaskIndex >= 0) {
        // 计算已专注的时间
        int focusedTime = workTime - remainingTime;  // 计算这次专注了多少时间
        if (focusedTime > 0) {
            tasks[currentTaskIndex].totalSeconds += focusedTime;  // 增加总时长
            saveTasks();  // 保存任务
            updateStats();  // 更新统计信息
        }

        // 增加专注次数
        tasks[currentTaskIndex].totalFocus++;
        saveTasks();
        updateTaskList();
    }
    
    if (isWorkTime) {
        // 工作时间结束
        if (currentCycle == totalCycles) {
            timer->stop();  // 只在完全结束时停止计时器
            showNotification("番茄钟", QString("恭喜！完成了所有%1次专注！").arg(totalCycles));
            // 播放完成音效
            if (enableCompleteSound && !completeSoundPath.isEmpty()) {
                playSound(completeSound);
            }
            endPomodoro();
            return;
        }
        // 切换到休息时间
        isWorkTime = false;
        remainingTime = breakTime;
        showNotification("番茄钟", "工作时间结束，开始休息！");
        // 播放休息音效
        if (enableBreakSound && !breakSoundPath.isEmpty()) {
            playSound(breakSound);
        }
        circularProgress->setColor(QColor(33, 150, 243));  // 休息时间用蓝色
    } else {
        // 休息时间结束
        isWorkTime = true;
        currentCycle++;
        remainingTime = workTime;
        showNotification("番茄钟", "休息时间结束，开始专注！");
        // 播放专注音效
        if (enableWorkSound && !workSoundPath.isEmpty()) {
            playSound(workSound);
        }
        circularProgress->setColor(QColor(76, 175, 80));  // 工作时间用绿色
    }
    
    // 保持按钮状态
    ui->startButton->setEnabled(false);  // 计时器正在运行时，开始按钮应该保持禁用
    ui->pauseButton->setEnabled(true);   // 暂停按钮应该可用
    ui->resetButton->setEnabled(false);  // 重置按钮在运行时应该禁用
    
    // 更新状态标签
    if (isWorkTime) {
        ui->statusLabel->setText(QString("正在专注 (%1/%2)")
            .arg(currentCycle)
            .arg(totalCycles));
    } else {
        ui->statusLabel->setText(QString("休息时间 (%1/%2)")
            .arg(currentCycle)
            .arg(totalCycles - 1));
    }
    
    // 不需要重新启动计时器，因为它一直在运行
    isRunning = true;
    updateDisplay();
}

void MainWindow::endPomodoro()
{
    timer->stop();
    isRunning = false;
    ui->startButton->setEnabled(true);
    ui->startButton->setText("开始");  // 任务完成后显示"开始"
    ui->pauseButton->setEnabled(false);
    
    // 启用时间设置控件
    ui->workMinSpinBox->setEnabled(true);
    ui->breakMinSpinBox->setEnabled(true);
    ui->cyclesSpinBox->setEnabled(true);
    
    // 启用任务管理相关控件
    ui->taskList->setEnabled(true);
    ui->taskComboBox->setEnabled(true);
    ui->addTaskButton->setEnabled(true);
    ui->editTaskButton->setEnabled(true);
    ui->deleteTaskButton->setEnabled(true);
    
    // 启用设置相关控件
    ui->tabWidget->setTabEnabled(1, true);  // 启用任务管理标签页
    ui->tabWidget->setTabEnabled(2, true);  // 启用时间统计标签页
    ui->tabWidget->setTabEnabled(3, true);  // 启用设置标签页
    
    // 重置为初始状态
    isWorkTime = true;
    currentCycle = 1;
    remainingTime = workTime;
    ui->statusLabel->setText(QString("正在专注 (1/%1)").arg(totalCycles));
    updateDisplay();
}

void MainWindow::loadSettings()
{
    QSettings settings("YourCompany", "Pomodoro");
    
    // 直接初始化为默认值
    workTime = 25 * 60;  // 默认25分钟
    breakTime = 5 * 60;  // 默认5分钟
    totalCycles = 4;      // 默认4个循环
    
    // 更新设置界面的值
    ui->workMinSpinBox->setValue(workTime / 60);  // 只显示分钟
    ui->breakMinSpinBox->setValue(breakTime / 60);  // 只显示分钟
    ui->cyclesSpinBox->setValue(totalCycles);
    
    // 更新进度条配置
    taskProgress->setTimes(workTime, breakTime);
    taskProgress->setTotalCycles(totalCycles);
    
    currentCycle = 1;
    remainingTime = workTime;
    ui->statusLabel->setText(QString("正在专注 (1/%1)").arg(totalCycles));
    updateDisplay();
    
    // 加载音效设置
    workSoundPath = settings.value("workSound").toString();
    breakSoundPath = settings.value("breakSound").toString();
    completeSoundPath = settings.value("completeSound").toString();
    enableWorkSound = settings.value("enableWorkSound", true).toBool();
    enableBreakSound = settings.value("enableBreakSound", true).toBool();
    enableCompleteSound = settings.value("enableCompleteSound", true).toBool();
    
    // 更新音效界面
    if (!workSoundPath.isEmpty()) {
        workSound->setSource(QUrl::fromLocalFile(workSoundPath));
        ui->workSoundEdit->setText(workSoundPath);
    }
    
    if (!breakSoundPath.isEmpty()) {
        breakSound->setSource(QUrl::fromLocalFile(breakSoundPath));
        ui->breakSoundEdit->setText(breakSoundPath);
    }
    
    if (!completeSoundPath.isEmpty()) {
        completeSound->setSource(QUrl::fromLocalFile(completeSoundPath));
        ui->completeSoundEdit->setText(completeSoundPath);
    }
    
    ui->enableWorkSoundCheck->setChecked(enableWorkSound);
    ui->enableBreakSoundCheck->setChecked(enableBreakSound);
    ui->enableCompleteSoundCheck->setChecked(enableCompleteSound);
}

void MainWindow::saveSettings()
{
    qDebug() << "saveSettings start";  // 调试输出
    // 获取设置值
    workTime = ui->workMinSpinBox->value() * 60;  // 只使用分钟
    breakTime = ui->breakMinSpinBox->value() * 60;  // 只使用分钟
    totalCycles = ui->cyclesSpinBox->value();
    
    if (workTime < 1) workTime = 1;
    if (breakTime < 1) breakTime = 1;
    
    // 保存设置
    QSettings settings("YourCompany", "Pomodoro");
    settings.setValue("workTime", workTime);
    settings.setValue("breakTime", breakTime);
    settings.setValue("totalCycles", totalCycles);
    
    // 更新进度条
    taskProgress->setTimes(workTime, breakTime);
    taskProgress->setTotalCycles(totalCycles);
    
    // 如果计时器没有运行，则更新剩余时间
    if (!isRunning) {
        // 重置状态
        isWorkTime = true;
        currentCycle = 1;
        remainingTime = workTime;
        updateDisplay();
    }
    
    // 保存音效设置
    settings.setValue("workSound", workSoundPath);
    settings.setValue("breakSound", breakSoundPath);
    settings.setValue("completeSound", completeSoundPath);
    settings.setValue("enableWorkSound", enableWorkSound);
    settings.setValue("enableBreakSound", enableBreakSound);
    settings.setValue("enableCompleteSound", enableCompleteSound);
}

void MainWindow::showNotification(const QString &title, const QString &message)
{
    trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 3000);
}

void MainWindow::playSound(QMediaPlayer *player)
{
    if (player->source().isValid()) {
        player->setPosition(0);
        player->play();
    }
}

void MainWindow::loadTasks()
{
    QSettings settings("YourCompany", "Pomodoro");
    int size = settings.beginReadArray("tasks");
    tasks.clear();
    
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString name = settings.value("name").toString();
        int totalFocus = settings.value("totalFocus", 0).toInt();
        int totalSeconds = settings.value("totalSeconds", 0).toInt();
        
        Task task(name);
        task.totalFocus = totalFocus;
        task.totalSeconds = totalSeconds;
        tasks.append(task);
    }
    settings.endArray();
    
    updateTaskList();
}

void MainWindow::saveTasks()
{
    QSettings settings("YourCompany", "Pomodoro");
    settings.beginWriteArray("tasks");
    
    for (int i = 0; i < tasks.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("name", tasks[i].name);
        settings.setValue("totalFocus", tasks[i].totalFocus);
        settings.setValue("totalSeconds", tasks[i].totalSeconds);
    }
    settings.endArray();
}

void MainWindow::updateTaskList()
{
    // 更新任务列表
    ui->taskList->clear();
    ui->taskComboBox->clear();
    
    for (const Task &task : tasks) {
        // 只显示任务名称
        ui->taskList->addItem(task.name);
        ui->taskComboBox->addItem(task.name);
    }
    
    // 恢复之前的选择
    if (currentTaskIndex >= 0 && currentTaskIndex < tasks.size()) {
        ui->taskList->setCurrentRow(currentTaskIndex);
        ui->taskComboBox->setCurrentIndex(currentTaskIndex);
    }
    
    // 更新按钮状态
    bool hasSelection = ui->taskList->currentRow() >= 0;
    ui->editTaskButton->setEnabled(hasSelection);
    ui->deleteTaskButton->setEnabled(hasSelection);
    ui->startButton->setEnabled(currentTaskIndex >= 0);
    
    // 更新统计信息
    updateStats();
}

void MainWindow::addNewTask()
{
    bool ok;
    QString name = QInputDialog::getText(this, 
        QString::fromUtf8("添加任务"),
        QString::fromUtf8("任务名称:"), 
        QLineEdit::Normal, "", &ok);
    
    if (ok && !name.isEmpty()) {
        Task task(name);
        tasks.append(task);
        updateTaskList();
        saveTasks();
    }
}

void MainWindow::editTask()
{
    int row = ui->taskList->currentRow();
    if (row < 0) return;
    
    TaskEditDialog dialog(tasks[row], this);
    if (dialog.exec() == QDialog::Accepted) {
        tasks[row].name = dialog.getName();
        tasks[row].totalSeconds += dialog.getAddSeconds();  // 添加时长而不是设置
        updateTaskList();
        saveTasks();
    }
}

void MainWindow::deleteTask()
{
    int row = ui->taskList->currentRow();
    if (row < 0) return;
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("删除任务"),
        tr("确定要删除这个任务吗？"), 
        QMessageBox::Yes|QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        tasks.removeAt(row);
        updateTaskList();
        saveTasks();
    }
}

void MainWindow::onTaskSelected(int index)
{
    if (index < 0 || index >= tasks.size()) return;
    
    currentTaskIndex = index;
}

void MainWindow::updateStats()
{
    // 计算总时间
    int totalSeconds = 0;
    for (const Task &task : tasks) {
        totalSeconds += task.totalSeconds;
    }
    
    // 更新总时间标签
    double totalHours = totalSeconds / 3600.0;  // 转换为小时，保留小数
    ui->totalTimeLabel->setText(QString("%1小时")
        .arg(totalHours, 0, 'f', 1));
    
    // 对任务按时间降序排序
    QVector<Task> sortedTasks = tasks;
    std::sort(sortedTasks.begin(), sortedTasks.end(),
        [](const Task &a, const Task &b) {
            return a.totalSeconds > b.totalSeconds;
        });
    
    // 清除旧的任务项
    QLayoutItem* item;
    while ((item = ui->taskItemsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    
    // 创建一个容器来包含所有任务项
    QWidget* taskContainer = new QWidget;
    QVBoxLayout* containerLayout = new QVBoxLayout(taskContainer);
    containerLayout->setSpacing(8);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    
    // 添加新的任务项
    for (int i = 0; i < sortedTasks.size(); i++) {
        const Task &task = sortedTasks[i];
        if (task.totalSeconds > 0) {  // 只显示有时间记录的任务
            double percent = static_cast<double>(task.totalSeconds) / totalSeconds;
            double taskHours = task.totalSeconds / 3600.0;
            
            // 创建任务项容器
            QWidget* taskItem = new QWidget;
            taskItem->setFixedHeight(50);  // 固定每个任务项的高度
            QVBoxLayout* taskLayout = new QVBoxLayout(taskItem);
            taskLayout->setSpacing(2);
            taskLayout->setContentsMargins(0, 0, 0, 0);
            
            // 创建信息行
            QWidget* infoWidget = new QWidget;
            QHBoxLayout* infoLayout = new QHBoxLayout(infoWidget);
            infoLayout->setContentsMargins(0, 0, 0, 0);
            
            // 任务名称（左对齐）
            QString nameText = QString("%1. %2").arg(i + 1).arg(task.name);
            QLabel* nameLabel = new QLabel(nameText);
            nameLabel->setStyleSheet("font-size: 14px;");
            
            // 时间和百分比（右对齐）
            QString statsText = QString("%1小时 (%2%)")
                .arg(taskHours, 0, 'f', 1)
                .arg(percent * 100, 0, 'f', 1);
            QLabel* statsLabel = new QLabel(statsText);
            statsLabel->setStyleSheet("font-size: 13px; color: #666;");
            
            infoLayout->addWidget(nameLabel);
            infoLayout->addStretch();
            infoLayout->addWidget(statsLabel);
            
            // 创建进度条
            QProgressBar* progressBar = new QProgressBar;
            progressBar->setMaximum(1000);  // 使用1000以获得更平滑的显示
            progressBar->setValue(percent * 1000);
            progressBar->setTextVisible(false);
            progressBar->setFixedHeight(6);  // 稍微增加进度条高度
            progressBar->setStyleSheet(
                "QProgressBar {"
                "  background-color: #f0f0f0;"
                "  border: none;"
                "  border-radius: 2px;"
                "  margin: 2px 0px;"  // 添加上下边距
                "}"
                "QProgressBar::chunk {"
                "  background-color: #4CAF50;"  // 使用绿色
                "  border-radius: 2px;"
                "}"
            );
            
            taskLayout->addWidget(infoWidget);
            taskLayout->addWidget(progressBar);
            
            containerLayout->addWidget(taskItem);
        }
    }
    
    // 添加弹性空间
    containerLayout->addStretch();
    
    // 将容器添加到滚动区域的布局中
    ui->taskItemsLayout->addWidget(taskContainer);
}
