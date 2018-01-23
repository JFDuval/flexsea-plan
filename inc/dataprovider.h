#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QObject>
#include <quantdata.h>
#include <QList>

class DataProvider : public QObject
{
	Q_OBJECT

public:
	explicit DataProvider() {}
	virtual ~DataProvider() {}

	virtual QuantData getNewestData() = 0;
	virtual QList<QuantData> getXMostRecent(int x) = 0;
	virtual QList<QuantData> getDataRange(int rangeStart, int rangeEnd) = 0;
	virtual QString getLabel() = 0;

	int getNumFields() {	return numFields;	}

signals:
	void dataAvailable(void);

protected:
	int numFields;

};

#endif // DATAPROVIDER_H
