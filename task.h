#ifndef TASK_H
#define TASK_H

#include <QString>

class Task
{
public:
    Task(const QString &name = "")
        : name(name) {}
    
    QString name;        // 任务名称
    int totalFocus = 0;  // 专注次数（保留，用于统计）
    int totalSeconds = 0; // 专注总时长（秒）
};

#endif // TASK_H 