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

#include "themewidget.h"
#include "ui_themewidget.h"

#include <QtCharts>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtCore/QRandomGenerator>
#include <QtCharts/QBarCategoryAxis>
#include <QtWidgets/QApplication>
#include <QtCharts/QValueAxis>

ThemeWidget::ThemeWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui_ThemeWidgetForm)
{
    m_ui->setupUi(this);
    populateThemeBox();
    populateAnimationBox();
    populateLegendBox();
    QChartView *chartView;

    // Declarations
    chart = new QChart();
    sinSeries = new QLineSeries(chart);
    noiseSeries = new QLineSeries(chart);
    timer = new QTimer();

    // Generating normal distribution and initial data
    updateDist();
    generateInitialData(xAxisRange);

    // Setting initial config 
    chart->setTitle("Sin(t) with Gaussian noise");
    sinSeries->setName("Sin(t)");
    noiseSeries->setName("Gaussian noise");
    chart->addSeries(noiseSeries);
    chart->addSeries(sinSeries);
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).first()->setRange(0, xAxisRange );
    chart->axes(Qt::Vertical).first()->setRange(-amplitude -1, amplitude + 1);
    QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
    Q_ASSERT(axisY);

    chartView = new QChartView(chart);
    m_ui->gridLayout->addWidget(chartView, 1, 2);
    m_charts << chartView;

    // Set defaults
    m_ui->antialiasCheckBox->setChecked(true);

    // Set the colors from the light theme as default ones
    QPalette pal = qApp->palette();
    pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
    pal.setColor(QPalette::WindowText, QRgb(0x404044));
    qApp->setPalette(pal);
    updateUI();

    // Setting timer tick, assigning the function and starting
    timer->setInterval(newPointSpeedMiliSecond); 
    connect(timer,SIGNAL(timeout()), this, SLOT(newPoint()));
    timer->start();
}

void ThemeWidget::newPoint()
{
    // Updating x Value and calculating new sin(t) value
    currentX += newPointDistanceIncrease;
    qreal sinValue = amplitude*qSin(currentX);

    // Creating new points
    QPointF sinPoint(currentX, sinValue);
    QPointF noisePoint(currentX, sinValue + (*dist)(generator));

    // Adding points to series
    sinSeries->append(sinPoint);
    noiseSeries->append(noisePoint);

    // Setting new x-Axis range to cause sensor of motion
    chart->axes(Qt::Horizontal).first()->setRange(currentX - xAxisRange, currentX);

    // Deleting old points to control used memory
    sinSeries->removePoints(0,1);
    noiseSeries->removePoints(0,1);
}

ThemeWidget::~ThemeWidget()
{
    delete m_ui;
}

void ThemeWidget::generateInitialData(const int _xAxisRange)
{
    qreal sinValue;
    for (currentX = 0; currentX < _xAxisRange; currentX += newPointDistanceIncrease)
    {
        sinValue = amplitude*qSin(currentX);

        QPointF sinPoint(currentX, sinValue);
        QPointF noisePoint(currentX, sinValue + (*dist)(generator));

        sinSeries->append(sinPoint);
        noiseSeries->append(noisePoint);
    }
}

void ThemeWidget::populateThemeBox()
{
    // add items to theme combobox
    m_ui->themeComboBox->addItem("Light", QChart::ChartThemeLight);
    m_ui->themeComboBox->addItem("Blue Cerulean", QChart::ChartThemeBlueCerulean);
    m_ui->themeComboBox->addItem("Dark", QChart::ChartThemeDark);
    m_ui->themeComboBox->addItem("Brown Sand", QChart::ChartThemeBrownSand);
    m_ui->themeComboBox->addItem("Blue NCS", QChart::ChartThemeBlueNcs);
    m_ui->themeComboBox->addItem("High Contrast", QChart::ChartThemeHighContrast);
    m_ui->themeComboBox->addItem("Blue Icy", QChart::ChartThemeBlueIcy);
    m_ui->themeComboBox->addItem("Qt", QChart::ChartThemeQt);
}

void ThemeWidget::populateAnimationBox()
{
    // add items to animation combobox
    m_ui->animatedComboBox->addItem("No Animations", QChart::NoAnimation);
    m_ui->animatedComboBox->addItem("GridAxis Animations", QChart::GridAxisAnimations);
}

void ThemeWidget::populateLegendBox()
{
    // add items to legend combobox
    m_ui->legendComboBox->addItem("No Legend ", 0);
    m_ui->legendComboBox->addItem("Legend Top", Qt::AlignTop);
    m_ui->legendComboBox->addItem("Legend Bottom", Qt::AlignBottom);
    m_ui->legendComboBox->addItem("Legend Left", Qt::AlignLeft);
    m_ui->legendComboBox->addItem("Legend Right", Qt::AlignRight);
}

void ThemeWidget::updateUI()
{
    QChart::ChartTheme theme = static_cast<QChart::ChartTheme>(
                m_ui->themeComboBox->itemData(m_ui->themeComboBox->currentIndex()).toInt());
    const auto charts = m_charts;
    if (!m_charts.isEmpty() && m_charts.at(0)->chart()->theme() != theme) {
        for (QChartView *chartView : charts) {
            chartView->chart()->setTheme(theme);
        }

        // Set palette colors based on selected theme
        QPalette pal = window()->palette();
        if (theme == QChart::ChartThemeLight) {
            pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else if (theme == QChart::ChartThemeDark) {
            pal.setColor(QPalette::Window, QRgb(0x121218));
            pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        } else if (theme == QChart::ChartThemeBlueCerulean) {
            pal.setColor(QPalette::Window, QRgb(0x40434a));
            pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        } else if (theme == QChart::ChartThemeBrownSand) {
            pal.setColor(QPalette::Window, QRgb(0x9e8965));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else if (theme == QChart::ChartThemeBlueNcs) {
            pal.setColor(QPalette::Window, QRgb(0x018bba));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else if (theme == QChart::ChartThemeHighContrast) {
            pal.setColor(QPalette::Window, QRgb(0xffab03));
            pal.setColor(QPalette::WindowText, QRgb(0x181818));
        } else if (theme == QChart::ChartThemeBlueIcy) {
            pal.setColor(QPalette::Window, QRgb(0xcee7f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else {
            pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        }
        window()->setPalette(pal);
    }

    // Update antialiasing
    bool checked = m_ui->antialiasCheckBox->isChecked();
    for (QChartView *chart : charts)
        chart->setRenderHint(QPainter::Antialiasing, checked);

    // Update animation options
    QChart::AnimationOptions options(
                m_ui->animatedComboBox->itemData(m_ui->animatedComboBox->currentIndex()).toInt());
    if (!m_charts.isEmpty() && m_charts.at(0)->chart()->animationOptions() != options) {
        for (QChartView *chartView : charts)
            chartView->chart()->setAnimationOptions(options);
    }

    // Update legend alignment
    Qt::Alignment alignment(
                m_ui->legendComboBox->itemData(m_ui->legendComboBox->currentIndex()).toInt());

    if (!alignment) {
        for (QChartView *chartView : charts)
            chartView->chart()->legend()->hide();
    } else {
        for (QChartView *chartView : charts) {
            chartView->chart()->legend()->setAlignment(alignment);
            chartView->chart()->legend()->show();
        }
    }
}

void ThemeWidget::togglePause()
{
    if(timer->isActive())
        timer->stop();
    else
        timer->start();
}

void ThemeWidget::amplitudeChange(double _newValue)
{
    amplitude = _newValue;
    chart->axes(Qt::Vertical).first()->setRange(-amplitude -1, amplitude + 1);
}

void ThemeWidget::meanChange(double _newValue)
{
    mean = _newValue;
    updateDist();
}

void ThemeWidget::stdDevChange(double _newValue)
{
    stdDev = _newValue;
    updateDist();
}

void ThemeWidget::updateDist()
{
    dist = new std::normal_distribution<double>(mean, stdDev);
}
