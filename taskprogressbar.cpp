#include "taskprogressbar.h"
#include <QPainter>
#include <QLinearGradient>
#include <QPainterPath>

TaskProgressBar::TaskProgressBar(QWidget *parent)
    : QWidget(parent)
    , totalCycles(4)
    , workTimeSeconds(25 * 60)
    , breakTimeSeconds(5 * 60)
    , totalTimeSeconds(0)
    , currentTimeSeconds(0)
    , isWorkTime(true)
    , m_currentProgress(0)
    , updateIntervalMs(20)  // 默认20ms
    , workColor(QColor(76, 175, 80, 180))
    , breakColor(QColor(33, 150, 243, 180))
    , backgroundColor(240, 240, 240)
{
    setMinimumHeight(24);
    setMaximumHeight(24);

    // 创建动画
    progressAnimation = new QPropertyAnimation(this, "currentProgress", this);
    progressAnimation->setDuration(updateIntervalMs);
    progressAnimation->setEasingCurve(QEasingCurve::Linear);

    // 计算总时间
    totalTimeSeconds = (workTimeSeconds * totalCycles) + (breakTimeSeconds * (totalCycles - 1));
}

void TaskProgressBar::setUpdateInterval(int ms)
{
    updateIntervalMs = ms;
    if (progressAnimation) {
        progressAnimation->setDuration(updateIntervalMs);
    }
}

void TaskProgressBar::setTotalCycles(int cycles)
{
    totalCycles = cycles;
    totalTimeSeconds = (workTimeSeconds * totalCycles) + (breakTimeSeconds * (totalCycles - 1));
    update();
}

void TaskProgressBar::setTimes(int workSeconds, int breakSeconds)
{
    workTimeSeconds = workSeconds;
    breakTimeSeconds = breakSeconds;
    totalTimeSeconds = (workTimeSeconds * totalCycles) + (breakTimeSeconds * (totalCycles - 1));
    update();
}

void TaskProgressBar::updateProgress(int currentSeconds, bool workTime)
{
    currentTimeSeconds = currentSeconds;
    isWorkTime = workTime;

    // 计算新的进度值
    qreal newProgress = (currentTimeSeconds * 100.0) / totalTimeSeconds;

    // 启动动画
    progressAnimation->stop();
    progressAnimation->setStartValue(m_currentProgress);
    progressAnimation->setEndValue(newProgress);
    progressAnimation->start();
}

void TaskProgressBar::setCurrentProgress(qreal progress)
{
    m_currentProgress = progress;
    update();
}

bool TaskProgressBar::isWorkTimeSegment(qreal progress) const
{
    int totalTime = workTimeSeconds + breakTimeSeconds;
    qreal cycleProgress = (progress * totalTimeSeconds) / 100.0;
    int currentCycle = cycleProgress / totalTime;
    qreal cycleTime = cycleProgress - (currentCycle * totalTime);
    return cycleTime <= workTimeSeconds;
}

void TaskProgressBar::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int radius = height() / 2;

    // 绘制背景
    QPainterPath backgroundPath;
    backgroundPath.addRoundedRect(rect(), radius, radius);
    painter.fillPath(backgroundPath, backgroundColor);

    if (m_currentProgress > 0) {
        // 计算进度条宽度
        int progressWidth = width() * (m_currentProgress / 100.0);

        // 创建进度条路径
        QPainterPath progressPath;
        QRectF progressRect(0, 0, progressWidth, height());

        // 添加圆角
        progressPath.addRoundedRect(progressRect, radius, radius);

        // 创建渐变
        QLinearGradient gradient(QPointF(0, 0), QPointF(0, height()));
        
        // 根据当前时间段设置颜色
        QColor color = isWorkTimeSegment(m_currentProgress) ? workColor : breakColor;
        gradient.setColorAt(0, color.lighter(110));
        gradient.setColorAt(1, color.darker(110));

        // 绘制进度
        painter.fillPath(progressPath, gradient);
    }

    // 添加细微的边框效果
    painter.setPen(QPen(QColor(0, 0, 0, 30), 1));
    painter.drawRoundedRect(rect().adjusted(0, 0, -1, -1), radius, radius);
}

void TaskProgressBar::setColors(const QColor &work, const QColor &rest)
{
    workColor = work;
    breakColor = rest;
    update();
} 