#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFileDialog>
#include "circularprogress.h"
#include <QPushButton>
#include "taskprogressbar.h"
#include "task.h"
#include "taskeditdialog.h"
#include <QProgressBar>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartButtonClicked();
    void onPauseButtonClicked();
    void onResetButtonClicked();
    void updateTimer();

private:
    Ui::MainWindow *ui;
    QVector<Task> tasks;          // 任务列表
    int currentTaskIndex = -1;    // 当前选择的任务索引
    QTimer *timer;
    QSystemTrayIcon *trayIcon;
    QMediaPlayer *workSound;      // 专注音效
    QMediaPlayer *breakSound;     // 休息音效
    QMediaPlayer *completeSound;  // 完成音效
    QAudioOutput *workAudio;      // 专注音频输出
    QAudioOutput *breakAudio;     // 休息音频输出
    QAudioOutput *completeAudio;  // 完成音频输出
    QString workSoundPath;        // 专注音效文件路径
    QString breakSoundPath;       // 休息音效文件路径
    QString completeSoundPath;    // 完成音效文件路径
    bool enableWorkSound;         // 是否启用专注音效
    bool enableBreakSound;        // 是否启用休息音效
    bool enableCompleteSound;     // 是否启用完成音效
    int remainingTime;    // 剩余时间（秒）
    bool isWorkTime;      // 是否处于工作时间
    bool isRunning;       // 计时器是否在运行
    
    int workTime;         // 工作时间（秒）
    int breakTime;        // 休息时间（秒）
    int totalCycles;      // 总循环次数
    int currentCycle;     // 当前循环次数
    
    CircularProgress *circularProgress;
    
    TaskProgressBar *taskProgress;
    
    void updateDisplay();
    void switchMode();    // 切换工作/休息模式
    void loadSettings();  // 加载设置
    void saveSettings();  // 保存设置
    void loadTasks();            // 加载任务列表
    void saveTasks();            // 保存任务列表
    void updateTaskList();       // 更新任务列表显示
    void updateStats();      // 更新统计信息
    void addNewTask();           // 添加新任务
    void deleteTask();           // 删除任务
    void editTask();             // 编辑任务
    void onTaskSelected(int index); // 任务选择响应
    void showNotification(const QString &title, const QString &message);
    void endPomodoro();   // 添加结束函数
    void playSound(QMediaPlayer *player);
};
#endif // MAINWINDOW_H
