#ifndef CHART_H
#define CHART_H

#include <QApplication>
#include <QWidget>
#include <QString>
#include <QTime>
#include <QTimer>
#include <QList>
#include <QScrollBar>
#include <QPoint>
#include "constants.h"

class Chart : public QWidget {
	Q_OBJECT
public:
	explicit Chart(QWidget* parent = 0);
	~Chart();
	void					paintEvent(QPaintEvent* event);

	QString					getName();
	void					setName(const QString& n);

	void					addChartData(const QString& dataName, QList<Data*>* chartData);

	void					setGrouping(const int g);

	void					setMinMaxDate(const QDateTime& minDate, const QDateTime& maxDate);

private:
	QString					_name;
	QList<QString>			_dataNames;
	QList<QList<Data*>*>	_chartData;
	int						_grouping;

	int						_xChartBegin;
	int						_xChartEnd;
	int						_yChartBegin;
	int						_yChartEnd;
	int						_chartWidth;
	int						_chartHeight;
	qlonglong				_minYval;
	qlonglong				_maxYval;
	qlonglong				_minXval;
	qlonglong				_maxXval;
	QDateTime				_minDate;
	QDateTime				_maxDate;
	int						_maxXpoints;
	int						_maxYpoints;
	int						_maxDataCount;
	QTime					_lastRepaintTime;
	QTimer					_repaintTimer;
	double					_xFactor;
	double					_yFactor;
	QPoint					_zeroPoint;

	void					drawChartBackground();
	void					drawAxes();
	void					drawGrid();
	void					drawChartName();
	void					drawDataLines();
	void					calculateChartSize();
	void					findMinMaxValues();

private slots:

signals:
	
public slots:
	
};

#endif // CHART_H
