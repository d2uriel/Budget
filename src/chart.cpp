#include "chart.h"
#include "household.h"
#include <QPaintEvent>
#include <QPainter>
#include <QColor>
#include <limits>

Chart::Chart(QWidget* parent) : QWidget(parent) {

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setMinimumSize(MIN_CHART_WIDTH, MIN_CHART_HEIGHT);
	_xChartBegin = CHART_MARGIN;
	_yChartBegin = CHART_MARGIN;
	_minYval = std::numeric_limits<qlonglong>::max();
	_maxYval = 0;
	_minXval = std::numeric_limits<qlonglong>::max();
	_maxXval = 0;
	_maxDataCount = 0;
	_grouping = CHART_GROUPING_MONTH;
	_zeroPoint.setX(0); _zeroPoint.setY(0);

	_lastRepaintTime = QTime::currentTime();
	_lastRepaintTime.start();

	connect(&_repaintTimer, SIGNAL(timeout()), this, SLOT(repaint()));
}

Chart::~Chart() {
}

QString Chart::getName() {
	return _name;
}

void Chart::setName(const QString& n) {
	_name = n;
}

void Chart::setGrouping(const int g) {
	_grouping = g;
}

void Chart::setMinMaxDate(const QDateTime& minDate, const QDateTime& maxDate) {
	_minDate = minDate;
	_maxDate = maxDate;
}

void Chart::paintEvent(QPaintEvent* event) {
	// Calculate chart size.
	calculateChartSize();
	// Draw chart background.
	drawChartBackground();
	// Draw axes.
	drawAxes();
	// Draw chart name.
	drawChartName();
	// If the user holds any mouse button don't draw the lines.
	if(_lastRepaintTime.elapsed() > 10) {
		// Find chart data min and max;
		findMinMaxValues();
		// Draw grid.
		drawGrid();
		// Draw data lines.
		drawDataLines();
		_lastRepaintTime.restart();
		if(_repaintTimer.isActive()) {
			_repaintTimer.stop();
		}
	} else {
		if(!_repaintTimer.isActive()) {
			_repaintTimer.start(100);
		}
	}
}

void Chart::addChartData(const QString &dataName, QList<Data*>* chartData) {
	_dataNames.append(dataName);
	_chartData.append(chartData);
}

void Chart::drawChartBackground() {
	QPainter painter(this);
	painter.save();

	QPen pen(QColor(Qt::white));
	painter.setPen(pen);
	QBrush brush(Qt::SolidPattern);
	brush.setColor(Qt::white);
	painter.setBrush(brush);
	painter.drawRect(0, 0, _xChartEnd + CHART_MARGIN, _yChartEnd + CHART_MARGIN);

	painter.restore();
}

void Chart::drawAxes() {
	QPainter painter(this);
	painter.save();

	QPen pen(QColor(Qt::black));
	painter.setPen(pen);
	// Draw vertical axis.
	painter.drawLine(_xChartBegin, _yChartBegin, _xChartBegin, _yChartEnd);
	// Draw horizontal axis.
	painter.drawLine(_xChartBegin, _yChartEnd, _xChartEnd, _yChartEnd);
	// Draw arrows for vertical axis.
	painter.drawLine(_xChartBegin, _yChartBegin, _xChartBegin - 5, _yChartBegin + 5);
	painter.drawLine(_xChartBegin, _yChartBegin, _xChartBegin + 5, _yChartBegin + 5);
	// Draw arrows for horizontal axis.
	painter.drawLine(_xChartEnd, _yChartEnd, _xChartEnd - 5, _yChartEnd - 5);
	painter.drawLine(_xChartEnd, _yChartEnd, _xChartEnd - 5, _yChartEnd + 5);

	painter.restore();
}

void Chart::drawGrid() {
	QPainter painter(this);
	painter.save();

	int spacingX = _chartWidth / _maxDataCount;
	if(spacingX < MIN_SPACING_X_AXIS) {
		spacingX = MIN_SPACING_X_AXIS;
	}
	//int spacingY = _chartHeight / (_maxYval - (_maxYval - _minYval));
	// Draw x grad.
	for(int x = _xChartBegin + spacingX; x < _xChartEnd - spacingX; x += spacingX) {
		painter.drawLine(x, _yChartEnd - 2, x, _yChartEnd + 2);
	}
	// Draw y grad.
	for(int y = _yChartEnd - MIN_SPACING_Y_AXIS; y > _yChartBegin + MIN_SPACING_Y_AXIS; y -= MIN_SPACING_Y_AXIS) {
		painter.drawLine(_xChartBegin - 2, y, _xChartBegin + 2, y);
	}

	painter.restore();
}

void Chart::drawChartName() {
	QPainter painter(this);
	painter.save();
	QFont chartNameFont("Arial", 16, QFont::Bold);
	QFontMetrics metrics(chartNameFont);
	QRect chartNameRect(metrics.boundingRect(_name));
	painter.setFont(chartNameFont);
	painter.drawText(QPoint((width() / 2) - (chartNameRect.width() / 2), 5 + chartNameRect.height()), _name);
	painter.restore();
}

void Chart::drawDataLines() {
	QPainter painter(this);
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing);
	QPen pen(QColor(Qt::black));
	painter.setPen(pen);
	for(int i = 0; i < _dataNames.count(); ++i) {
		// Draw legend.
		// TBD.
		// Draw data lines.
		if(_chartData[i]->count() > 1) {
			for(int j = 0; j < _chartData[i]->count() - 1; ++j) {
				Data* d = _chartData[i]->at(j);
				Data* nd = _chartData[i]->at(j + 1);
				int x1 = _xChartBegin + (j * _xFactor);
				int y1 = _yChartEnd - (d->value / _yFactor);
				int x2 = _xChartBegin + ((j + 1) * _xFactor);
				int y2 = _yChartEnd - (nd->value / _yFactor);
				painter.drawLine(x1, y1, x2, y2);
			}
		}
	}
	painter.restore();
}

void Chart::calculateChartSize() {
	_xChartEnd = width() - CHART_MARGIN;
	_yChartEnd = height() - CHART_MARGIN;
	_chartWidth = _xChartEnd - _xChartBegin;
	_chartHeight = _yChartEnd - _yChartBegin;
	_maxXpoints = (_xChartEnd - _xChartBegin - MIN_SPACING_X_AXIS) / MIN_SPACING_X_AXIS;
	_maxYpoints = (_yChartEnd - _yChartBegin - MIN_SPACING_Y_AXIS) / MIN_SPACING_Y_AXIS;
}

void Chart::findMinMaxValues() {
	_maxDataCount = 0;
	for(int i = 0; i < _chartData.count(); ++i) {
		for(int j = 0; j < _chartData[i]->count(); ++j) {
			if(_chartData[i]->count() > _maxDataCount) {
				_maxDataCount = _chartData[i]->count();
			}
			Data* d = _chartData[i]->at(j);
			if(d->value < _minYval) {
				_minYval = d->value;
			}
			if(d->value > _maxYval) {
				_maxYval = d->value;
			}
			if(d->dateTime.toMSecsSinceEpoch() < _minXval) {
				_minXval = d->dateTime.toMSecsSinceEpoch();
			}
			if(d->dateTime.toMSecsSinceEpoch() > _maxXval) {
				_maxXval = d->dateTime.toMSecsSinceEpoch();
			}
		}
	}
	if(_maxDataCount < _maxXpoints) {
		_xFactor = _chartWidth / _maxDataCount;
	} else {
		_xFactor = MIN_SPACING_X_AXIS;
	}
	_yFactor = (_maxYval / _maxYpoints) / MIN_SPACING_Y_AXIS;
}
