#include "circularprogress.h"
#include <QPainter>
#include <QPainterPath>
#include <QDebug>

CircularProgress::CircularProgress(QWidget *parent)
    : QWidget(parent)
    , progress(0)
    , m_currentProgress(0)
    , progressColor(QColor(76, 175, 80))  // 默认使用绿色
{
    setMinimumSize(200, 200);  // 设置最小尺寸
    
    // 创建动画对象
    animation = new QPropertyAnimation(this, "currentProgress", this);
    animation->setDuration(300);  // 设置动画持续时间为300毫秒
    animation->setEasingCurve(QEasingCurve::OutCubic);  // 设置缓动曲线
}

void CircularProgress::setProgress(int value)
{
    progress = value;
    
    // 启动动画
    animation->stop();
    animation->setStartValue(m_currentProgress);
    animation->setEndValue(value);
    animation->start();
}

void CircularProgress::setCurrentProgress(qreal progress)
{
    m_currentProgress = progress;
    update();  // 触发重绘
}

void CircularProgress::setTimeText(const QString &text)
{
    timeText = text;
    update();
}

void CircularProgress::setColor(const QColor &color)
{
    progressColor = color;
    update();
}

void CircularProgress::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 计算圆的尺寸和位置
    int size = qMin(width(), height());
    int outerRadius = size / 2 - 10;
    int innerRadius = outerRadius - 20;

    // 移动坐标系到中心
    painter.translate(width() / 2, height() / 2);

    // 绘制外圈（灰色背景）
    QPen pen(Qt::NoPen);
    painter.setPen(pen);
    painter.setBrush(QColor(200, 200, 200));
    painter.drawEllipse(-outerRadius, -outerRadius, outerRadius * 2, outerRadius * 2);

    // 绘制内圈（白色背景）
    painter.setBrush(Qt::white);
    painter.drawEllipse(-innerRadius, -innerRadius, innerRadius * 2, innerRadius * 2);

    // 绘制进度圆环
    if (m_currentProgress > 0) {
        painter.setBrush(progressColor);
        int progressRadius = outerRadius;
        
        // 创建一个路径来绘制圆环扇形
        QPainterPath path;
        path.moveTo(0, 0);
        
        // 从12点钟方向开始，顺时针方向
        path.lineTo(0, -progressRadius);
        path.arcTo(-progressRadius, -progressRadius, 
                  progressRadius * 2, progressRadius * 2, 
                  90, -m_currentProgress * 360.0 / 100.0);
        path.lineTo(0, 0);

        // 使用裁剪区域创建圆环效果
        QPainterPath ringPath;
        ringPath.addEllipse(-outerRadius, -outerRadius, outerRadius * 2, outerRadius * 2);
        ringPath.addEllipse(-innerRadius, -innerRadius, innerRadius * 2, innerRadius * 2);

        painter.setClipPath(ringPath);
        painter.drawPath(path);
        painter.setClipping(false);
    }

    // 绘制时间文本
    QFont font = painter.font();
    font.setPointSize(24);  // 增大字号
    painter.setFont(font);
    painter.setPen(Qt::black);
    QRect textRect(-innerRadius, -20, innerRadius * 2, 40);
    painter.drawText(textRect, Qt::AlignCenter, timeText);
} 