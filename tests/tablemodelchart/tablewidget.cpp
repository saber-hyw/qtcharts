/****************************************************************************
**
** Copyright (C) 2012 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the Qt Commercial Charts Add-on.
**
** $QT_BEGIN_LICENSE$
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.digia.com
** $QT_END_LICENSE$
**
****************************************************************************/

#include "tablewidget.h"
#include <QGridLayout>
#include <QTableView>
#include <QChart>
#include <QStyledItemDelegate>
#include <QLineSeries>
#include <QSplineSeries>
#include <QScatterSeries>
#include <QVXYModelMapper>
#include <QHXYModelMapper>
#include "customtablemodel.h"
#include <QPieSeries>
#include <QVPieModelMapper>
#include <QPieSlice>
#include <QAreaSeries>
#include <QBarSeries>
#include <QGroupedBarSeries>
#include <QBarSet>
#include <QVBarModelMapper>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include <QSpinBox>
#include <QTime>
#include <QHeaderView>

TableWidget::TableWidget(QWidget *parent)
    : QWidget(parent),
      m_series(0),
      m_mapper(0),
      m_model(0),
      m_pieMapper(0),
      m_pieMapper2(0)
    //      specialPie(0)
{
    setGeometry(1900, 100, 1000, 600);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    // create simple model for storing data
    // user's table data model
    m_model = new CustomTableModel;
    m_tableView = new QTableView;
    m_tableView->setModel(m_model);
    //    m_tableView->setMinimumHeight(300);
    m_tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_tableView->verticalHeader()->setResizeMode(QHeaderView::Stretch);

    m_chart = new QChart;
    m_chart->legend()->setVisible(true);
    m_chart->setAnimationOptions(QChart::SeriesAnimations);
    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setMinimumSize(640, 480);

    // add, remove data buttons
    QPushButton* addRowAboveButton = new QPushButton("Add row above");
    connect(addRowAboveButton, SIGNAL(clicked()), this, SLOT(addRowAbove()));

    QPushButton* addRowBelowButton = new QPushButton("Add row below");
    connect(addRowBelowButton, SIGNAL(clicked()), this, SLOT(addRowBelow()));

    QPushButton* removeRowButton = new QPushButton("Remove row");
    connect(removeRowButton, SIGNAL(clicked()), this, SLOT(removeRow()));

    QPushButton* addColumnRightButton = new QPushButton("Add column to the right");
    connect(addColumnRightButton, SIGNAL(clicked()), this, SLOT(addColumnRight()));

    QPushButton* removeColumnButton = new QPushButton("Remove column");
    connect(removeColumnButton, SIGNAL(clicked()), this, SLOT(removeColumn()));

    QPushButton* specialPieButton = new QPushButton("Add slices using series API");
    connect(specialPieButton, SIGNAL(clicked()), this, SLOT(testPie()));

    QPushButton* specialPieButton2 = new QPushButton("Remove slices using series API");
    connect(specialPieButton2, SIGNAL(clicked()), this, SLOT(testPie2()));

    QPushButton* specialPieButton3 = new QPushButton("Modify slices using series API");
    connect(specialPieButton3, SIGNAL(clicked()), this, SLOT(testPie3()));

    QPushButton* xyTestButton = new QPushButton("Append XY point");
    connect(xyTestButton, SIGNAL(clicked()), this, SLOT(testXY()));


    QLabel *spinBoxLabel = new QLabel("Rows affected:");

    // spin box for setting number of affected items (add, remove)
    m_linesCountSpinBox = new QSpinBox;
    m_linesCountSpinBox->setRange(1, 10);
    m_linesCountSpinBox->setValue(1);

    // buttons layout
    QVBoxLayout* buttonsLayout = new QVBoxLayout;
    buttonsLayout->addWidget(spinBoxLabel);
    buttonsLayout->addWidget(m_linesCountSpinBox);
    //    buttonsLayout->addWidget(addRowAboveButton);
    buttonsLayout->addWidget(addRowBelowButton);
    buttonsLayout->addWidget(removeRowButton);
    //    buttonsLayout->addWidget(addColumnRightButton);
    //    buttonsLayout->addWidget(removeColumnButton);
    buttonsLayout->addWidget(specialPieButton);
    buttonsLayout->addWidget(specialPieButton2);
    buttonsLayout->addWidget(specialPieButton3);
    buttonsLayout->addWidget(xyTestButton);
    buttonsLayout->addStretch();

    // chart type radio buttons
    m_lineRadioButton = new QRadioButton("Line");
    m_splineRadioButton = new QRadioButton("Spline");
    m_scatterRadioButton = new QRadioButton("Scatter");
    m_pieRadioButton = new QRadioButton("Pie");
    m_areaRadioButton = new QRadioButton("Area");
    m_barRadioButton = new QRadioButton("Bar");

    connect(m_lineRadioButton, SIGNAL(toggled(bool)), this, SLOT(updateChartType(bool)));
    connect(m_splineRadioButton, SIGNAL(toggled(bool)), this, SLOT(updateChartType(bool)));
    connect(m_scatterRadioButton, SIGNAL(toggled(bool)), this, SLOT(updateChartType(bool)));
    connect(m_pieRadioButton, SIGNAL(toggled(bool)), this, SLOT(updateChartType(bool)));
    connect(m_areaRadioButton, SIGNAL(toggled(bool)), this, SLOT(updateChartType(bool)));
    connect(m_barRadioButton, SIGNAL(toggled(bool)), this, SLOT(updateChartType(bool)));
    m_lineRadioButton->setChecked(true);

    // radio buttons layout
    QVBoxLayout* radioLayout = new QVBoxLayout;
    radioLayout->addWidget(m_lineRadioButton);
    radioLayout->addWidget(m_splineRadioButton);
    radioLayout->addWidget(m_scatterRadioButton);
    radioLayout->addWidget(m_pieRadioButton);
    //    radioLayout->addWidget(m_areaRadioButton);
    radioLayout->addWidget(m_barRadioButton);
    radioLayout->addStretch();

    // create main layout
    QGridLayout* mainLayout = new QGridLayout;
    mainLayout->addLayout(buttonsLayout, 2, 0);
    mainLayout->addLayout(radioLayout, 3, 0);
    mainLayout->addWidget(m_tableView, 1, 0);
    mainLayout->addWidget(m_chartView, 1, 1, 2, 1);
    setLayout(mainLayout);
    m_lineRadioButton->setFocus();
}

void TableWidget::addRowAbove()
{
    m_model->insertRows(m_tableView->currentIndex().row(), m_linesCountSpinBox->value());

}

void TableWidget::addRowBelow()
{
    m_model->insertRows(m_tableView->currentIndex().row() + 1, m_linesCountSpinBox->value());

}

void TableWidget::removeRow()
{
    m_model->removeRows(m_tableView->currentIndex().row(), qMin(m_model->rowCount() - m_tableView->currentIndex().row(), m_linesCountSpinBox->value()));
}

void TableWidget::addColumnRight()
{
    m_model->insertColumns(m_tableView->currentIndex().column() + 1, m_linesCountSpinBox->value());
}

void TableWidget::removeColumn()
{
    m_model->removeColumns(m_tableView->currentIndex().column(), qMin(m_model->columnCount() - m_tableView->currentIndex().column(), m_linesCountSpinBox->value()));
}

void TableWidget::updateChartType(bool toggle)
{
    // this if is needed, so that the function is only called once.
    // For the radioButton that was enabled.
    if (toggle) {
        //        specialPie = 0;
        m_chart->removeAllSeries();
        m_series = 0;
        //        m_chart->axisX()->setNiceNumbersEnabled(false);
        //        m_chart->axisY()->setNiceNumbersEnabled(false);
        if (m_mapper) {
            m_mapper->deleteLater();
            m_mapper = 0;
        }

        if (m_pieMapper) {
            m_pieMapper->deleteLater();
            m_pieMapper = 0;
        }

        if (m_pieMapper2) {
            m_pieMapper2->deleteLater();
            m_pieMapper2 = 0;
        }

        //        if (m_series) {
        //            delete m_series;
        //            m_series = 0;
        //        }

        // renable axes of the chart (pie hides them)
        // x axis
        QAxis *axis = m_chart->axisX();
        axis->setAxisVisible(true);
        axis->setGridLineVisible(true);
        axis->setLabelsVisible(true);

        // y axis
        axis = m_chart->axisY();
        axis->setAxisVisible(true);
        axis->setGridLineVisible(true);
        axis->setLabelsVisible(true);

        m_model->clearMapping();

        QString seriesColorHex = "#000000";
        //        QPen pen;
        //        pen.setWidth(2);

        if (m_lineRadioButton->isChecked())
        {
//            m_chart->setAnimationOptions(QChart::NoAnimation);

            // series 1
            m_series = new QLineSeries;

//            m_mapper = new QHXYModelMapper;
//            m_mapper->setModel(m_model);
//            m_mapper->setSeries(m_series);
//            m_mapper->setXRow(0);
//            m_mapper->setYRow(1);
//            m_mapper->setFirst(3);
//            m_mapper->setCount(4);

            m_mapper = new QVXYModelMapper;
            m_mapper->setModel(m_model);
            m_mapper->setSeries(m_series);
            m_mapper->setXColumn(0);
            m_mapper->setYColumn(1);
            m_mapper->setFirst(3);
//            m_mapper->setCount(4);

            //            m_series->setModelMapping(0,1, Qt::Vertical);
            //            m_series->setModelMappingRange(3, 4);
            m_chart->addSeries(m_series);
            seriesColorHex = "#" + QString::number(m_series->pen().color().rgb(), 16).right(6).toUpper();
            m_model->addMapping(seriesColorHex, QRect(0, 3, 2, 4));

            //            // series 2
            //            m_series = new QLineSeries;
            //            m_series->setModel(m_model);

            //            mapper = new QXYModelMapper;
            //            mapper->setMapX(3);
            //            mapper->setMapY(4);
            //            //            mapper->setFirst(3);
            //            //            mapper->setCount(4);
            //            m_series->setModelMapper(mapper);
            //            //            m_series->setModelMapping(2,3, Qt::Vertical);
            //            m_chart->addSeries(m_series);
            //            seriesColorHex = "#" + QString::number(m_series->pen().color().rgb(), 16).right(6).toUpper();
            //            m_model->addMapping(seriesColorHex, QRect(3, 0, 2, 1000));

            //            // series 3
            //            m_series = new QLineSeries;
            //            m_series->setModel(m_model);

            //            mapper = new QXYModelMapper;
            //            mapper->setMapX(5);
            //            mapper->setMapY(6);
            //            mapper->setFirst(2);
            //            mapper->setCount(-1);
            //            m_series->setModelMapper(mapper);
            //            //            m_series->setModelMapping(4,5, Qt::Vertical);
            //            //            m_series->setModelMappingRange(2, -1);
            //            m_chart->addSeries(m_series);
            //            seriesColorHex = "#" + QString::number(m_series->pen().color().rgb(), 16).right(6).toUpper();
            //            m_model->addMapping(seriesColorHex, QRect(5, 2, 2, 1000));
        }
        else if (m_splineRadioButton->isChecked())
        {
            m_chart->setAnimationOptions(QChart::NoAnimation);

            // series 1
            m_series = new QSplineSeries;
            //            m_series->setModel(m_model);

//            m_mapper = new QVXYModelMapper;
//            m_mapper->setSeries(m_series);
//            m_mapper->setModel(m_model);
//            m_mapper->setXColumn(0);
//            m_mapper->setYColumn(1);
//            m_mapper->setFirst(0);
//            m_mapper->setCount(-1);

            //            m_series->setModelMapper(mapper);

            m_chart->addSeries(m_series);
            seriesColorHex = "#" + QString::number(m_series->pen().color().rgb(), 16).right(6).toUpper();
            m_model->addMapping(seriesColorHex, QRect(0, 0, 2, 1000));

            //            // series 2
            //            m_series = new QSplineSeries;
            //            m_series->setModel(m_model);

            //            mapper = new QXYModelMapper;
            //            mapper->setMapX(2);
            //            mapper->setMapY(3);
            //            mapper->setFirst(2);
            //            mapper->setCount(4);

            //            m_series->setModelMapper(mapper);

            //            m_chart->addSeries(m_series);
            //            seriesColorHex = "#" + QString::number(m_series->pen().color().rgb(), 16).right(6).toUpper();
            //            m_model->addMapping(seriesColorHex, QRect(2, 2, 2, 4));

            //            // series 3
            //            m_series = new QSplineSeries;
            //            m_series->setModel(m_model);

            //            mapper = new QXYModelMapper;
            //            mapper->setMapX(4);
            //            mapper->setMapY(5);
            //            mapper->setFirst(2);
            //            mapper->setCount(-1);

            //            m_series->setModelMapper(mapper);

            //            m_chart->addSeries(m_series);
            //            seriesColorHex = "#" + QString::number(m_series->pen().color().rgb(), 16).right(6).toUpper();
            //            m_model->addMapping(seriesColorHex, QRect(4, 2, 2, 1000));
        }        else if (m_scatterRadioButton->isChecked())
        {
            m_chart->setAnimationOptions(QChart::NoAnimation);

            // series 1
            m_series = new QScatterSeries;

//            m_mapper = new QVXYModelMapper;
//            m_mapper->setSeries(m_series);
//            m_mapper->setModel(m_model);
//            m_mapper->setXColumn(0);
//            m_mapper->setYColumn(1);
//            m_mapper->setFirst(0);
//            m_mapper->setCount(-1);

            m_chart->addSeries(m_series);
            seriesColorHex = "#" + QString::number(m_series->brush().color().rgb(), 16).right(6).toUpper();
            m_model->addMapping(seriesColorHex, QRect(0, 2, 2, 1000));

            //            // series 2
            //            m_series = new QScatterSeries;
            //            m_series->setModel(m_model);
            //            m_series->setModelMapping(2,3, Qt::Vertical);
            //            //            m_series->setModelMappingRange(1, 6);
            //            //        series->setModelMapping(2,3, Qt::Horizontal);
            //            m_chart->addSeries(m_series);

            //            seriesColorHex = "#" + QString::number(m_series->brush().color().rgb(), 16).right(6).toUpper();
            //            m_model->addMapping(seriesColorHex, QRect(2, 1, 2, 6));

            //            // series 3
            //            m_series = new QScatterSeries;
            //            m_series->setModel(m_model);
            //            m_series->setModelMapping(4,5, Qt::Vertical);
            //            //        series->setModelMapping(4,5, Qt::Horizontal);
            //            m_chart->addSeries(m_series);
            //            seriesColorHex = "#" + QString::number(m_series->brush().color().rgb(), 16).right(6).toUpper();
            //            m_model->addMapping(seriesColorHex, QRect(4, 0, 2, 1000));
        } else if (m_pieRadioButton->isChecked()) {
            m_chart->setAnimationOptions(QChart::SeriesAnimations);

            // pie 1
            m_pieSeries = new QPieSeries;

            m_pieMapper = new QVPieModelMapper;
            m_pieMapper->setValuesColumn(1);
            m_pieMapper->setLabelsColumn(7);
            m_pieMapper->setSeries(m_pieSeries);
            m_pieMapper->setModel(m_model);
            m_pieMapper->setFirst(2);
            //                        m_pieMapper->setCount(5);
            //                    pieSeries->setModelMapper(mapper);

            m_pieSeries->setLabelsVisible(true);
            m_pieSeries->setPieSize(0.35);
            m_pieSeries->setHorizontalPosition(0.25);
            m_pieSeries->setVerticalPosition(0.35);

            m_chart->addSeries(m_pieSeries);
            seriesColorHex = "#" + QString::number(m_pieSeries->slices().at(m_pieSeries->slices().count()/2)->brush().color().rgb(), 16).right(6).toUpper();
            m_model->addMapping(seriesColorHex, QRect(1, 2, 1, 50));


            //                            pieSeries->slices().at(0)->setValue(400);
            //                            pieSeries->slices().at(0)->setLabel(QString("36"));

            // pie 2
            m_pieSeries2 = new QPieSeries;

            m_pieMapper2 = new QVPieModelMapper;
            m_pieMapper2->setValuesColumn(0);
            m_pieMapper2->setLabelsColumn(7);
            m_pieMapper2->setModel(m_model);
            m_pieMapper2->setSeries(m_pieSeries2);
            m_pieMapper2->setFirst(2);

            m_pieSeries2->setLabelsVisible(true);
            m_pieSeries2->setPieSize(0.35);
            m_pieSeries2->setHorizontalPosition(0.75);
            m_pieSeries2->setVerticalPosition(0.65);
            m_chart->addSeries(m_pieSeries2);
            seriesColorHex = "#" + QString::number(m_pieSeries2->slices().at(m_pieSeries2->slices().count()/2)->brush().color().rgb(), 16).right(6).toUpper();
            m_model->addMapping(seriesColorHex, QRect(0, 2, 1, 1000));

            //            // pie 3
            //            pieSeries = new QPieSeries;
            //            pieSeries->setModel(m_model);
            //            pieSeries->setModelMapping(2,2, Qt::Vertical);
            //            pieSeries->setLabelsVisible(true);
            //            pieSeries->setPieSize(0.35);
            //            pieSeries->setHorizontalPosition(0.5);
            //            pieSeries->setVerticalPosition(0.75);
            //            m_chart->addSeries(pieSeries);
            //            seriesColorHex = "#" + QString::number(pieSeries->slices().at(pieSeries->slices().count()/2)->brush().color().rgb(), 16).right(6).toUpper();
            //            m_model->addMapping(seriesColorHex, QRect(2, 0, 1, 1000));

            //            // special pie
            //            specialPie = new QPieSeries;
            //            specialPie->append(17, "1");
            //            specialPie->append(45, "2");
            //            specialPie->append(77, "3");
            //            specialPie->append(37, "4");
            //            specialPie->append(27, "5");
            //            specialPie->append(47, "6");
            //            specialPie->setPieSize(0.35);
            //            specialPie->setHorizontalPosition(0.8);
            //            specialPie->setVerticalPosition(0.75);
            //            specialPie->setLabelsVisible(true);
            //            m_chart->addSeries(specialPie);
        }
        //        else if (m_areaRadioButton->isChecked())
        //        {
        //            m_chart->setAnimationOptions(QChart::NoAnimation);

        //            QLineSeries* upperLineSeries = new QLineSeries;
        //            upperLineSeries->setModel(m_model);
        //            upperLineSeries->setModelMapping(0, 1, Qt::Vertical);
        //            //            upperLineSeries->setModelMappingRange(1, 5);
        //            QLineSeries* lowerLineSeries = new QLineSeries;
        //            lowerLineSeries->setModel(m_model);
        //            lowerLineSeries->setModelMapping(2, 3, Qt::Vertical);
        //            QAreaSeries* areaSeries = new QAreaSeries(upperLineSeries, lowerLineSeries);
        //            m_chart->addSeries(areaSeries);
        //            seriesColorHex = "#" + QString::number(areaSeries->brush().color().rgb(), 16).right(6).toUpper();
        //            m_model->addMapping(seriesColorHex, QRect(0, 1, 2, 5));
        //            m_model->addMapping(seriesColorHex, QRect(2, 0, 2, 1000));
        //        }
        else if (m_barRadioButton->isChecked())
        {
            m_chart->setAnimationOptions(QChart::SeriesAnimations);

            QGroupedBarSeries* barSeries = new QGroupedBarSeries();
//            barSeries->setCategories(QStringList());
//            barSeries->setModel(m_model);
            //            barSeries->setModelMappingRange(2, 5);
            //                    barSeries->setModelMapping(5, 2, 4, Qt::Vertical);

            int first = 3;
//            int count = 4;
            QVBarModelMapper *mapper = new QVBarModelMapper;
            mapper->setCategoriesSection(5);
            mapper->setFirstBarSetSection(2);
            mapper->setLastBarSetSection(4);
            mapper->setFirst(first);
//            mapper->setCount(count);
            mapper->setSeries(barSeries);
            mapper->setModel(m_model);
//            barSeries->setModelMapper(mapper);
            m_chart->addSeries(barSeries);
            QList<QBarSet*> barsets = barSeries->barSets();
            for (int i = 0; i < barsets.count(); i++) {
                seriesColorHex = "#" + QString::number(barsets.at(i)->brush().color().rgb(), 16).right(6).toUpper();
                m_model->addMapping(seriesColorHex, QRect(2 + i, first, 1, barsets.at(i)->count()));
            }
        }


        if (!m_barRadioButton->isChecked()) {
//            m_chart->axisX()->setRange(0, 500);
//            m_chart->axisY()->setRange(0, 220);
        }
        m_chart->legend()->setVisible(true);

        // repaint table view colors
        m_tableView->repaint();
        m_tableView->setFocus();
    }
}

void TableWidget::testPie()
{
    //    m_pieMapper->setCount(-1);
    QPieSlice *slice = new QPieSlice("Hehe", 145);
    slice->setLabelVisible();
    m_pieSeries->append(slice);

    slice = new QPieSlice("Hoho", 34);
    slice->setLabelVisible();
    m_pieSeries->append(slice);
    //    m_series->modelMapper()->setMapX(4);
    //    m_tableView->setColumnWidth(10, 250);
    //    if (specialPie) {
    //        specialPie->remove(specialPie->slices().at(2));
    //        //    specialPie->insert(4, new QPieSlice(45, "Hello"));//specialPie->slices.at(2));
    //        specialPie->append(4, "heloo");
    //    }
}

void TableWidget::testPie2()
{
    QPieSlice *slice;
    if (m_pieSeries->count() > 0) {
        slice = m_pieSeries->slices().last();
        m_pieSeries->remove(slice);
    }

    if (m_pieSeries->count() > 0) {
        slice = m_pieSeries->slices().first();
        m_pieSeries->remove(slice);
    }
}

void TableWidget::testPie3()
{
    QPieSlice *slice;
    if (m_pieSeries->count() > 0) {
        slice = m_pieSeries->slices().last();
        slice->setLabel("Dalej");
        slice->setValue(222);
    }

    if (m_pieSeries->count() > 0) {
        slice = m_pieSeries->slices().first();
        slice->setLabel("Prawie");
        slice->setValue(111);
    }
}

void TableWidget::testXY()
{
    //    if (m_series->type() != QAbstractSeries::SeriesTypeLine) {
    //        m_series->append(QPointF(150, 75));
    //    }

    if (m_series->count() > 0) {
        m_series->remove(m_series->points().last());
    }
}

TableWidget::~TableWidget()
{

}