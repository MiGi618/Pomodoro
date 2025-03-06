#ifndef CIRCULARPROGRESS_H
#define CIRCULARPROGRESS_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

class CircularProgress : public QWidget
{
    Q_OBJECT
    // 添加progress属性，使其可以被动画系统使用
    Q_PROPERTY(qreal currentProgress READ currentProgress WRITE setCurrentProgress)

public:
    explicit CircularProgress(QWidget *parent = nullptr);

    void setProgress(int value);  // 0-100
    void setTimeText(const QString &text);
    void setColor(const QColor &color);

    // 添加当前进度的getter和setter
    qreal currentProgress() const { return m_currentProgress; }
    void setCurrentProgress(qreal progress);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int progress;
    qreal m_currentProgress;
    QString timeText;
    QColor progressColor;
    QPropertyAnimation *animation;
};

#endif // CIRCULARPROGRESS_H 