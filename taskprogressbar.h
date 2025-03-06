#ifndef TASKPROGRESSBAR_H
#define TASKPROGRESSBAR_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QPainterPath>
#include <QColor>
#include <QTimer>

class TaskProgressBar : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal currentProgress READ currentProgress WRITE setCurrentProgress)

public:
    explicit TaskProgressBar(QWidget *parent = nullptr);

    void setTotalCycles(int cycles);
    void setTimes(int workSeconds, int breakSeconds);  // 设置工作和休息时间
    void updateProgress(int currentSeconds, bool isWorkTime);  // 更新当前进度
    void setColors(const QColor &workColor, const QColor &breakColor);
    void setUpdateInterval(int ms);  // 设置更新间隔

    // 动画相关
    qreal currentProgress() const { return m_currentProgress; }
    void setCurrentProgress(qreal progress);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int totalCycles;
    int workTimeSeconds;    // 工作时间（秒）
    int breakTimeSeconds;   // 休息时间（秒）
    int totalTimeSeconds;   // 总时间（秒）
    int currentTimeSeconds; // 当前已过时间（秒）
    bool isWorkTime;
    qreal m_currentProgress;  // 总进度（0-100）
    QColor workColor;
    QColor breakColor;
    QColor backgroundColor;
    QPropertyAnimation *progressAnimation;
    int updateIntervalMs;  // 更新间隔（毫秒）

    // 辅助方法
    bool isWorkTimeSegment(qreal progress) const;  // 判断某个进度点是否在工作时间段
    qreal calculateSegmentProgress(qreal totalProgress, int segmentIndex) const;  // 计算某个时间段的进度
};

#endif // TASKPROGRESSBAR_H 