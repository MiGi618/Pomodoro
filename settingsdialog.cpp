#include "settingsdialog.h"
#include <QGridLayout>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("设置"));

    // 创建控件
    workMinSpinBox = new QSpinBox(this);
    workSecSpinBox = new QSpinBox(this);
    breakMinSpinBox = new QSpinBox(this);
    breakSecSpinBox = new QSpinBox(this);
    cyclesSpinBox = new QSpinBox(this);
    
    // 设置范围
    workMinSpinBox->setRange(0, 60);
    workSecSpinBox->setRange(0, 59);
    breakMinSpinBox->setRange(0, 30);
    breakSecSpinBox->setRange(0, 59);
    cyclesSpinBox->setRange(1, 10);
    
    // 设置后缀
    workMinSpinBox->setSuffix("分");
    workSecSpinBox->setSuffix("秒");
    breakMinSpinBox->setSuffix("分");
    breakSecSpinBox->setSuffix("秒");
    
    // 添加进度条更新间隔设置
    updateIntervalSpinBox = new QSpinBox(this);
    updateIntervalSpinBox->setRange(10, 100);  // 10-100ms
    updateIntervalSpinBox->setSuffix("ms");
    
    // 创建按钮
    okButton = new QPushButton(tr("确定"), this);
    cancelButton = new QPushButton(tr("取消"), this);
    
    // 创建布局
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(new QLabel(tr("专注时间:")), 0, 0);
    gridLayout->addWidget(workMinSpinBox, 0, 1);
    gridLayout->addWidget(workSecSpinBox, 0, 2);
    
    gridLayout->addWidget(new QLabel(tr("休息时间:")), 1, 0);
    gridLayout->addWidget(breakMinSpinBox, 1, 1);
    gridLayout->addWidget(breakSecSpinBox, 1, 2);
    
    gridLayout->addWidget(new QLabel(tr("循环次数:")), 2, 0);
    gridLayout->addWidget(cyclesSpinBox, 2, 1);
    
    gridLayout->addWidget(new QLabel(tr("进度条更新间隔:")), 3, 0);
    gridLayout->addWidget(updateIntervalSpinBox, 3, 1);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(gridLayout);
    mainLayout->addLayout(buttonLayout);
    
    setLayout(mainLayout);
    
    // 连接信号和槽
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

// Getter 方法
int SettingsDialog::getWorkMinutes() const { return workMinSpinBox->value(); }
int SettingsDialog::getWorkSeconds() const { return workSecSpinBox->value(); }
int SettingsDialog::getBreakMinutes() const { return breakMinSpinBox->value(); }
int SettingsDialog::getBreakSeconds() const { return breakSecSpinBox->value(); }
int SettingsDialog::getCycles() const { return cyclesSpinBox->value(); }
int SettingsDialog::getProgressUpdateInterval() const 
{ 
    return updateIntervalSpinBox->value(); 
}

// Setter 方法
void SettingsDialog::setWorkTime(int minutes, int seconds) {
    workMinSpinBox->setValue(minutes);
    workSecSpinBox->setValue(seconds);
}

void SettingsDialog::setBreakTime(int minutes, int seconds) {
    breakMinSpinBox->setValue(minutes);
    breakSecSpinBox->setValue(seconds);
}

void SettingsDialog::setCycles(int cycles) {
    cyclesSpinBox->setValue(cycles);
}

void SettingsDialog::setProgressUpdateInterval(int ms) 
{
    updateIntervalSpinBox->setValue(ms);
} 