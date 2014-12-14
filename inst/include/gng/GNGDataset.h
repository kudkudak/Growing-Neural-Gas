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

#include "utils/threading.h"
#include "utils/utils.h"
#include "GNGGlobals.h"
#include "GNGConfiguration.h"

namespace gmum {

/** Database for growing neural gas interface
 *
 * Example is its interiors just an double array with relevant data at different positions.
 * What specifies
 *
 * @note Drawing example is not very time-expensive comapred to other problems (like
 * neighbour search). Therefore it is locking on addExample and drawExample
 *
 * @note Takes ownership of the memory inserted. Copy memory before inserting if you
 * want to use this memory afterwards!!
 */
class GNGDataset {
public:

	/*
	 * @returns Layout for example. We want examples to be laid in continous
	 * array of doubles for performance and storage convenience so basically
	 * an example can have any dimensionality and this function returns array
	 * with specified 3 checkpoints:
	 *    * Start of position data
	 *    * Start of vertex data (that will be majority voted in algorithm)
	 *    * Start of metadata (most likely probability of being sampled)
	 */
	virtual std::vector<int> getDataLayout() const=0;

	virtual void lock() = 0;
	virtual void unlock() = 0;

	virtual int getDataDim() const=0;

	virtual int getGNGDim() const =0;

	///Returns index of example drawn
	virtual unsigned int drawExample()=0;

	///Retrieves pointer to position
	virtual const double * getPosition(unsigned int) const=0;

	///Retrieves pointer to metadata, with unsigned int
	virtual const double * getMetaData(unsigned int) const=0;

	///Retrieves pointer to vertex data, with unsigned int as descriptor of meta
	virtual const double * getVertexData(unsigned int) const=0;

	///Inserts examples to the dataset
	virtual void insertExamples(void *, unsigned int count,
			unsigned int size)=0;

	//Set examples (takes ownership of the memory)
	virtual void setExamples(void *, unsigned int count, unsigned int size)=0;

	virtual void removeExample(unsigned int)=0;

	virtual int getSize() const=0;

	virtual const double * getMemoryPtr() const = 0;

	virtual ~GNGDataset() {
	}

	GNGDataset(const GNGDataset& orig) {
	}

	GNGDataset() {
	}

};





///Storage :< GNGDatabaseStorage
class GNGDatasetSimple: public GNGDataset {
protected:
	const unsigned int pos_dim_, extra_dim_;
	const unsigned int prob_location_;

	gmum::recursive_mutex * mutex_;


	vector<double> storage;
	vector<double> storage_extra;
	vector<double> storage_probability;


	std::vector<int> data_layout_;

	bool sampling_;
	unsigned int current_example_;

	boost::shared_ptr<Logger> m_logger;
public:

	/*
	 * @param prob_location If prob location is -1 (by default) means there
	 * is no probability data in meta data.
	 */
	GNGDatasetSimple(gmum::recursive_mutex *mutex, unsigned int pos_dim,
			unsigned int extra_dim, bool sampling = true,
			boost::shared_ptr<Logger> logger = boost::shared_ptr<Logger>()) :
			mutex_(mutex), pos_dim_(
					pos_dim), vertex_dim_(vertex_dim), meta_dim_(meta_dim), prob_location_(
					prob_location), sampling_(sampling), current_example_(0), m_logger(
					logger) {
		//Data layout
		data_layout_.push_back(pos_dim_);
		data_layout_.push_back(vertex_dim_);
		data_layout_.push_back(meta_dim_);

		assert(sampling_ or prob_location_ == -1);

		__init_rnd();
	}

	void lock() {
		mutex_->lock();
	}
	void unlock() {
		mutex_->unlock();
	}

	~GNGDatasetSimple() {
		DBG(m_logger,10, "GNGDatasetSimple:: destroying");
	}

	const double * getMemoryPtr() const {
		return storage_.getMemoryPtr();
	}

	///Retrieves pointer to position
	const typename Storage::baseType * getPosition(unsigned int i) const {

		return &storage_.getData(i)[0];
	}

	///Retrieves pointer to metadata, with unsigned int
	const typename Storage::baseType * getMetaData(unsigned int i) const {

		if (meta_dim_ == 0)
			return 0;

		return &storage_.getData(i)[pos_dim_ + vertex_dim_];
	}

	///Retrieves pointer to vertex data, with unsigned int as descriptor of meta
	const typename Storage::baseType * getVertexData(unsigned int i) const {

		if (vertex_dim_ == 0)
			return 0;

		return &storage_.getData(i)[pos_dim_];
	}

	unsigned int drawExample() {

		if (sampling_) {
			if (prob_location_ == -1) {
				return __int_rnd(0, storage_.getSize() - 1);
			} else {
				const double * ex;
				unsigned int index;

				//Sample
				do {
					index = __int_rnd(0, storage_.getSize() - 1);
					ex = storage_.getData(index);
				} while (ex[pos_dim_ + vertex_dim_ + prob_location_]
						< __double_rnd(0, 1.0));

				return index;
			}
		} else {
			unsigned int tmp = current_example_;

			current_example_ = (current_example_ + 1) % (storage_.getSize());

			//could be (current_eample_++)%(storage_.getSize()) probably :p

			return tmp;

		}
	}

	void insertExamples(void * memptr, unsigned int count, unsigned int size) {
		typename Storage::storageType * examples =
				reinterpret_cast<typename Storage::storageType *>(memptr);
		storage_.insertData(examples, count, size);
	}
	void setExamples(void * memptr, unsigned int count, unsigned int size) {
		typename Storage::storageType * examples =
				reinterpret_cast<typename Storage::storageType *>(memptr);
		storage_.setData(examples, count, size);
	}
virtual std::vector<int> getDataLayout() const {
		return data_layout_;
	}

	void removeExample(unsigned int index) {
		throw 1; //not implemented
	}
	int getSize() const {
		return storage_.getSize();
	}

	virtual int getDataDim() const {
		return pos_dim_ + meta_dim_ + vertex_dim_;
	}

	virtual int getGNGDim() const {
		return pos_dim_;
	}
private:
};
}

#endif
/* GNGEXAMPLEMANAGER_H */
