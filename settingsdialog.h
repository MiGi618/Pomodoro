#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    
    int getWorkMinutes() const;
    int getWorkSeconds() const;
    int getBreakMinutes() const;
    int getBreakSeconds() const;
    int getCycles() const;
    int getProgressUpdateInterval() const;
    
    void setWorkTime(int minutes, int seconds);
    void setBreakTime(int minutes, int seconds);
    void setCycles(int cycles);
    void setProgressUpdateInterval(int ms);

private:
    QSpinBox *workMinSpinBox;
    QSpinBox *workSecSpinBox;
    QSpinBox *breakMinSpinBox;
    QSpinBox *breakSecSpinBox;
    QSpinBox *cyclesSpinBox;
    QSpinBox *updateIntervalSpinBox;
    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif // SETTINGSDIALOG_H 