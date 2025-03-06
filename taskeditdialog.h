#ifndef TASKEDITDIALOG_H
#define TASKEDITDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include "task.h"

class TaskEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TaskEditDialog(const Task& task, QWidget *parent = nullptr);
    
    QString getName() const { return nameEdit->text(); }
    int getAddSeconds() const { return hoursSpinBox->value() * 3600 + minutesSpinBox->value() * 60; }
    int getCurrentSeconds() const { return m_currentSeconds; }

private:
    QLineEdit* nameEdit;
    QSpinBox* hoursSpinBox;
    QSpinBox* minutesSpinBox;
    int m_currentSeconds;
};

#endif // TASKEDITDIALOG_H 