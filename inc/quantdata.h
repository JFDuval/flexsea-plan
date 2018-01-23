#ifndef QUANTDATA_H
#define QUANTDATA_H

#define	QUANT_DATA_T long int

#include <vector>

typedef std::vector<QUANT_DATA_T> QuantData;

class TimeStampedData {
public:
	unsigned int timestamp;
	QUANT_DATA_T value;
};

//class QuantData
//{
//public:
//	QuantData(int numberOfFields = 0) {
//		numFields_ = numberOfFields;
//		data = std::vector<QUANT_DATA_T>(numFields_);
//		/*
//		if(numFields_ > 0)
//			data = new QUANT_DATA_T[numFields_];
//		else
//			data = nullptr;
//		*/
//	}

//	~QuantData() {
//		/*
//		if(data) delete [] data;
//		data = nullptr;
//		*/
//	}

//	int numFields() const			{	return numFields_;	}
//	std::vector<QUANT_DATA_T> data;

//private:
//	int numFields_;

//};

#endif // QUANTDATA_H
