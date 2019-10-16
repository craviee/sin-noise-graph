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
#include <QDebug>
#include <QPushButton>

ThemeWidget::ThemeWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui_ThemeWidgetForm)
{

    dataTable = new DataTable();
    sinDataList = new DataList();
    noiseDataList = new DataList();
    chart = new QChart();

    updateDist();

    generateRandomData(1, 10, 10);
    m_ui->setupUi(this);
    populateThemeBox();
    populateAnimationBox();
    populateLegendBox();
    series = new QLineSeries(chart);
    series2 = new QLineSeries(chart);
    //create charts
    
    QChartView *chartView;

    //![5]
    createLineChart();
    chartView = new QChartView(chart);
    m_ui->gridLayout->addWidget(chartView, 1, 2);
    //![5]
    m_charts << chartView;

    // Set defaults
    m_ui->antialiasCheckBox->setChecked(true);

    // Set the colors from the light theme as default ones
    QPalette pal = qApp->palette();
    pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
    pal.setColor(QPalette::WindowText, QRgb(0x404044));
    qApp->setPalette(pal);

    timer = new QTimer();
    timer->setInterval(10); //Time in milliseconds
    //timer->setSingleShot(false); //Setting this to true makes the timer run only once
    connect(timer, &QTimer::timeout, this, [=](){
        // dataTable[0].push_back(Data(QPointF(0,qSin(0)), ""));
        
        currentX += 0.01;
        qreal sinValue = amplitude*qSin(currentX);
        QPointF sinPoint(currentX, sinValue);
        QPointF noisePoint(currentX, sinValue + (*dist)(generator));
        // QString label = "Slice " + QString::number(i) + ":" + QString::number(j);
        // *sinDataList << Data(value, "");
        series->append(sinPoint);
        series2->append(noisePoint);
        // printf("currentX: %lf\n",currentX);
        chart->axes(Qt::Horizontal).first()->setRange(currentX-10, currentX);
        chart->axes(Qt::Vertical).first()->setRange(-amplitude - 1, amplitude + 1);
        series->removePoints(0,1);
        series2->removePoints(0,1);
        // chartView->repaint();
        // chart->axes(Qt).first()->setRange(0, currentX);
    });
    timer->start();
    updateUI();
//    togglePause();

}

ThemeWidget::~ThemeWidget()
{
    delete m_ui;
}

DataTable ThemeWidget::generateRandomData(int listCount, int valueMax, int valueCount)
{
    
    qreal sinValue;
    // generate random data
    // for (int i(0); i < listCount; i++) {
        qreal yValue(0);
        for (currentX =0; currentX < valueCount; currentX+=0.01) {
            // yValue = yValue + QRandomGenerator::global()->bounded(valueMax / (qreal) valueCount);
            // QPointF value((j + QRandomGenerator::global()->generateDouble()) * ((qreal) m_valueMax / (qreal) valueCount),
                        //   yValue);
            sinValue = amplitude*qSin(currentX);
            QPointF sinPoint(currentX, sinValue);
            // QPointF noisePoint(currentX, sinValue + (*dist)(generator));
            
            QPointF noisePoint(currentX, sinValue + (*dist)(generator));
            // QString label = "Slice " + QString::number(i) + ":" + QString::number(j);
            *sinDataList << Data(sinPoint, "");
            *noiseDataList << Data(noisePoint, "");
           // currentX = j;
        }
        *dataTable << *sinDataList;
        *dataTable << *noiseDataList;
    // }

    return *dataTable;
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

QChart *ThemeWidget::createLineChart()
{
    //![1]
    
    chart->setTitle("Sin(t) with noise");
    //![1]

    //![2]
    // QString name("Series ");
    // int nameIndex = 0;
    // for (const DataList &list : *dataTable) {
    for (const Data &data : *sinDataList)
        series->append(data.first);
    for (const Data &data : *noiseDataList)
        series2->append(data.first);
    series->setName("Sin(t)");
    series2->setName("Gaussian noise");
    
    chart->addSeries(series2);
    chart->addSeries(series);
    // }
    //![2]

    //![3]
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).first()->setRange(0, 20);
    chart->axes(Qt::Vertical).first()->setRange(-amplitude -1, amplitude + 1);
    //![3]
    //![4]
    // Add space to label to add space between labels and axis
    QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
    Q_ASSERT(axisY);
    axisY->setLabelFormat("%.1f  ");
    //![4]

    return chart;
}

void ThemeWidget::updateUI()
{
    //![6]
    QChart::ChartTheme theme = static_cast<QChart::ChartTheme>(
                m_ui->themeComboBox->itemData(m_ui->themeComboBox->currentIndex()).toInt());
    //![6]
    const auto charts = m_charts;
    if (!m_charts.isEmpty() && m_charts.at(0)->chart()->theme() != theme) {
        for (QChartView *chartView : charts) {
            //![7]
            chartView->chart()->setTheme(theme);
            //![7]
        }

        // Set palette colors based on selected theme
        //![8]
        QPalette pal = window()->palette();
        if (theme == QChart::ChartThemeLight) {
            pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        //![8]
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
    //![11]
    bool checked = m_ui->antialiasCheckBox->isChecked();
    for (QChartView *chart : charts)
        chart->setRenderHint(QPainter::Antialiasing, checked);
    //![11]

    // Update animation options
    //![9]
    QChart::AnimationOptions options(
                m_ui->animatedComboBox->itemData(m_ui->animatedComboBox->currentIndex()).toInt());
    if (!m_charts.isEmpty() && m_charts.at(0)->chart()->animationOptions() != options) {
        for (QChartView *chartView : charts)
            chartView->chart()->setAnimationOptions(options);
    }
    //![9]

    // Update legend alignment
    //![10]
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
    //![10]
}

void ThemeWidget::togglePause()
{
    if(timer->isActive())
        timer->stop();
    else
        timer->start();
}

void ThemeWidget::amplitudeChange(double newValue)
{
    amplitude = newValue;
    chart->axes(Qt::Vertical).first()->setRange(-amplitude -1, amplitude + 1);
}

void ThemeWidget::meanChange(double newValue)
{
    mean = newValue;
    updateDist();
}

void ThemeWidget::stdDevChange(double newValue)
{
    stdDev = newValue;
    updateDist();
}

void ThemeWidget::updateDist()
{
    dist = new std::normal_distribution<double>(mean, stdDev);
}
