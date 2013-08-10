
#ifndef _DATASET_HPP__
#define _DATASET_HPP__

#include <string>
#include <cassert>
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <set>

namespace HPCS
{

typedef unsigned int UInt;
typedef double Real;
  
class DataSet
{
public:

    typedef unsigned int UInt;

    typedef double Real;
  
    DataSet( const UInt & nbSamples, const UInt & nbPts );
    
    DataSet( Real * data, const UInt & nbSamples, const UInt & nPts );
    
    ~DataSet()
    {
      
      delete[] M_data; 

    };

    void readCSV( const std::string & filename );
    
    void writeCSV( std::ostream & output = std::cout ) const;
    
    void showMe( std::ostream & output = std::cout ) const;
    
    Real * getData(){ return M_data; };
    
    void setData( Real * data, const UInt & nbSamples, const UInt & nPts);
    
    void setOffset( const UInt & leftOffset, const UInt & rightOffset );
   
    Real operator()( const UInt & sample, const UInt & pt ) const;
    
    UInt nbSamples() const { return this->M_nbSamples; }
    
    UInt nbPts() const { return this->M_nbPts; }
    
protected:

  Real * M_data;

  UInt M_nbSamples;

  UInt M_nbPts;
  
  UInt M_leftOffset;
  
  UInt M_rightOffset;
  
};


class DataSetLevelled : public DataSet 
{
  
public:
  
  typedef unsigned int UInt;
  
  typedef double Real;
  
  typedef std::set< UInt > IDContainer_Type;
  
  typedef std::vector< IDContainer_Type > levelsContainer_Type;
  
  typedef boost::shared_ptr< levelsContainer_Type > levelsContainerPtr_Type;
  
  
  DataSetLevelled( const UInt & nbSamples, const UInt & nbPts, const UInt & nbLevels );
  
  DataSetLevelled( Real * data, const UInt & nbSamples, const UInt & nbPts, const UInt & nbLevels );
  
  ~DataSetLevelled();
  
  
  void setLevels( const levelsContainerPtr_Type & levelsPtr );
  
  void setLevels( const std::vector< UInt > & linearExtrema );
  

  void showMe( std::ostream & output = std::cout ) const;  
  
  IDContainer_Type & levelIDs( const UInt lev ) const;
  
  UInt cardinality( const UInt & levelID );
  
  
private:
  
  levelsContainerPtr_Type M_levelsPtr;
  
  UInt M_nbLevels;
  
  std::vector< UInt > M_cardinality;
  
  
};




}

#endif /* _DATASET_HPP__ */

