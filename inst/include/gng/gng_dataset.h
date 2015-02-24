/*
 * File: GNGExampleManager.h
 * Author: Stanisław "kudkudak" Jastrzebski
 *
 * Created on 11 sierpień 2012, 10:47
 */
#ifndef GNGDATABASE_H
#define GNGDATABASE_H

#include <algorithm>
#include <string.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <globals.h>
#include <gng_configuration.h>

#include "utils/threading.h"
#include "utils/utils.h"

namespace gmum {

/** Database for growing neural gas interface
 *
 * @note Drawing example is not very time-expensive comapred to other problems (like
 * neighbour search). Therefore it is locking on addExample and drawExample
 *
 * @note Takes ownership of the memory inserted. Copy memory before inserting if you
 * want to use this memory afterwards!!
 */
class GNGDataset {
public:

	virtual int getDataDim() const=0;

	virtual int getGNGDim() const =0;

	///Returns index of example drawn
	virtual unsigned int drawExample()=0;

	///Retrieves pointer to position
	virtual const double * getPosition(unsigned int) const=0;

	///Retrieves pointer to vertex data, with unsigned int as descriptor of meta
	virtual const double * getExtraData(unsigned int) const=0;

	///Inserts examples to the dataset
	virtual void insertExamples(const double *, const double*, const double *,
			unsigned int count)=0;

	virtual void removeExample(unsigned int)=0;

	virtual int size() const=0;

	virtual ~GNGDataset() {
	}

	virtual void lock() = 0;
	virtual void unlock() = 0;
};

///Storage :< GNGDatabaseStorage
template<typename T = double>
class GNGDatasetSimple: public GNGDataset {

protected:
	const unsigned int dim_;

	gmum::recursive_mutex * mutex_;

	vector<T> storage_;
	vector<T> storage_extra_;
	vector<T> storage_probability_;

	bool store_extra_;
	unsigned int current_example_;

	boost::shared_ptr<Logger> logger_;
public:

	enum AccessMethod {
		Sequential, Sampling, SamplingProbability
	} access_method_;

	/*
	 * @param prob_location If prob location is -1 (by default) means there
	 * is no probability data in meta data.
	 */
	GNGDatasetSimple(gmum::recursive_mutex *mutex, unsigned int dim,
			bool store_extra = false, AccessMethod access_method = Sequential,
			boost::shared_ptr<Logger> logger = boost::shared_ptr<Logger>()) :
			mutex_(mutex), store_extra_(store_extra), dim_(dim), access_method_(
					access_method), current_example_(0), logger_(logger) {
		__init_rnd();
	}

	void lock() {
		mutex_->lock();
	}
	void unlock() {
		mutex_->unlock();
	}

	~GNGDatasetSimple() {
		DBG(logger_,10, "GNGDatasetSimple:: destroying");
	}

	///Retrieves pointer to position
	const T * getPosition(unsigned int i) const {
		return &storage_[i * dim_];
	}

	const T * getExtraData(unsigned int i) const {
		if (!store_extra_)
			return 0;
		return &storage_extra_[i];
	}

	unsigned int drawExample() {

		if (access_method_ != Sequential) {
			if (access_method_ == Sampling) {
				return __int_rnd(0, size() - 1);
			} else {
				const double * ex;
				unsigned int index;

				do {
					index = __int_rnd(0, size() - 1);
					ex = getPosition(index);
				} while (storage_probability_[index] < __double_rnd(0, 1.0));

				return index;
			}
		} else {
			unsigned int tmp = current_example_;

			current_example_ = (current_example_ + 1) % size();

			return tmp;

		}
	}

	void insertExamples(const double * positions, const double *extra,
			const double *probability, unsigned int count) {

		if (storage_.capacity() < storage_.size() + count * dim_) {
			DBG(logger_,10, "Resizing storage_");
			storage_.reserve(storage_.size() + count * dim_);
			DBG(logger_,10, "Resized storage_");
		}

		storage_.insert(storage_.end(), positions, positions + count * dim_);

		if (store_extra_) {
			if (storage_extra_.capacity() < storage_extra_.size() + count) {
				DBG(logger_,10, "Resizing store_extra_");
				storage_extra_.reserve(storage_extra_.size() + count);
			}

			if (!extra) {
				for (size_t i = 0; i < count; ++i)
					storage_extra_.push_back(0);
			} else {
				storage_extra_.insert(storage_extra_.end(), extra,
						extra + count);
			}
		}
		if (access_method_ == SamplingProbability) {
			assert(probability);

			if (storage_probability_.capacity()
					< storage_probability_.size() + count) {
				DBG( logger_,10, "Resizing storage_probability_");
				storage_probability_.reserve(
						storage_probability_.size() + count);
			}

			storage_probability_.insert(storage_probability_.end(), probability,
					probability + count);
		}
	}

	void removeExample(unsigned int index) {
		throw BasicException("Not implemented");
	}

	int size() const {
		return storage_.size() / dim_;
	}

	virtual int getDataDim() const {
		return dim_;
	}

	virtual int getGNGDim() const {
		return dim_;
	}
private:
};
}

#endif
/* GNGEXAMPLEMANAGER_H */
