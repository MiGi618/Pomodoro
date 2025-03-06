#include "taskeditdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>

TaskEditDialog::TaskEditDialog(const Task& task, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("编辑任务");
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 任务名称
    QHBoxLayout* nameLayout = new QHBoxLayout;
    QLabel* nameLabel = new QLabel("任务名称:", this);
    nameEdit = new QLineEdit(task.name, this);
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    mainLayout->addLayout(nameLayout);
    
    // 添加时长
    QHBoxLayout* timeLayout = new QHBoxLayout;
    QLabel* timeLabel = new QLabel("添加时长:", this);
    hoursSpinBox = new QSpinBox(this);
    hoursSpinBox->setMaximum(999);
    hoursSpinBox->setValue(0);
    hoursSpinBox->setSuffix("小时");
    
    minutesSpinBox = new QSpinBox(this);
    minutesSpinBox->setMaximum(59);
    minutesSpinBox->setValue(0);
    minutesSpinBox->setSuffix("分钟");
    
    timeLayout->addWidget(timeLabel);
    timeLayout->addWidget(hoursSpinBox);
    timeLayout->addWidget(minutesSpinBox);
    mainLayout->addLayout(timeLayout);
    
    // 确定取消按钮
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
} 