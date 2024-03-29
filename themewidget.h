/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef THEMEWIDGET_H
#define THEMEWIDGET_H

#include <QtWidgets/QWidget>
#include <QtCharts>
#include <QtMath>
#include <QTimer>
#include <random>

QT_BEGIN_NAMESPACE
class QComboBox;
class QCheckBox;
class Ui_ThemeWidgetForm;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

typedef QList<QPointF> DataList;
typedef QList<DataList> DataTable;

QT_CHARTS_USE_NAMESPACE

class ThemeWidget: public QWidget
{
    Q_OBJECT
public:
    explicit ThemeWidget(QWidget *parent = 0);
    ~ThemeWidget();

private Q_SLOTS:
    void updateUI();

private slots:
    void togglePause();
    void amplitudeChange(double _newValue);
    void meanChange(double _newValue);
    void stdDevChange(double _newValue);
    void newPoint();

private:
    void generateInitialData(const int _xAxisRange);
    void populateThemeBox();
    void populateAnimationBox();
    void populateLegendBox();
    void updateDist();

private:
    
    const int xAxisRange = 10;
    const int newPointSpeedMiliSecond = 10;
    const double newPointDistanceIncrease = 0.01;

    double currentX;
    double amplitude = 1.0;
    double mean = 0.0;
    double stdDev = 0.1;

    std::default_random_engine generator;
    QList<QChartView *> m_charts;
    QChart *chart;
    QLineSeries *sinSeries;
    QLineSeries *noiseSeries;
    QTimer* timer;
    std::normal_distribution<double> *dist;
    
    Ui_ThemeWidgetForm *m_ui;
};

#endif /* THEMEWIDGET_H */
