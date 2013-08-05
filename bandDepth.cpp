
#include <mpi.h>

#include "bandDepth.hpp"
#include "dataSet.hpp"

namespace HPCS
{
  
 ///////////////////////////
 //	BD DATA
 //////////////////////////
  
 // Constructor from single variables.
 BandDepthData::
 BandDepthData( const UInt & nbPz, const UInt & nbPts, const UInt & leftOffset, 
	 const UInt & rightOffset, const UInt & J, const UInt & verbosity )
 :
 M_nbPz( nbPz ),
 M_nbPts( nbPts ),
 M_leftOffset( leftOffset ),
 M_rightOffset( rightOffset ),
 M_J( J ),
 M_verbosity( verbosity )
 {}
  
 // Constructor from Get Pot type object
 BandDepthData::
 BandDepthData( const data_Type & dataFile, const std::string & section )
 {
    const std::string baseName( section );
      
    this->M_nbPz =  dataFile( ( baseName + "/nbPz"  ).data(), 10 );
      
    const UInt nbPtsFull = dataFile( ( baseName + "/nbPts" ).data(), 100 );
      
    this->M_leftOffset = dataFile( ( baseName + "/leftOffset" ).data(), 0 );
      
    this->M_rightOffset = dataFile( ( baseName + "/rightOffset" ).data(), 0 );
      
    this->M_nbPts = nbPtsFull - M_leftOffset - M_rightOffset;
      
    this->M_J = dataFile( ( baseName + "/J" ).data(), 2 );

    this->M_verbosity = dataFile( ( baseName + "/verbosity" ).data(), false );
      
    this->M_inputFilename = dataFile( ( baseName + "/inputFilename" ).data(), "data.dat");
      
    this->M_outputFilename = dataFile( ( baseName + "/outputFilename" ).data(), "bd.dat" );
      
 }
 
 // Copy constructor
 BandDepthData::
 BandDepthData( const BandDepthData & bdData )
 {
    this->M_nbPz = bdData.nbPz();   
    
    this->M_leftOffset = bdData.leftOffset();
    
    this->M_rightOffset = bdData.leftOffset();
    
    this->M_nbPts = bdData.nbPts();
    
    this->M_J = bdData.J();
    
    this->M_verbosity = bdData.verbosity();
    
    this->M_inputFilename = bdData.inputFilename();
    
    this->M_outputFilename = bdData.outputFilename();
 }
 
 // Setter for the output filename
 void 
 BandDepthData::
 setInputFilename( const std::string & inputFilename )
 {
    this->M_inputFilename = inputFilename;
    
    return;
 }
 
 // Setter for the input filename  
 void 
 BandDepthData::
 setOutputFilename( const std::string & outputFilename )
 {
    this->M_outputFilename = outputFilename;
    
    return;
 }

 /////////////////////////
 // 	Band Depth
 /////////////////////////
 
 // Constructor from BandDepthData type object
 BandDepth::
 BandDepth( const bdData_Type & bdData )
 {
   this->M_bdDataPtr.reset( new bdData_Type( bdData ) );
   
   this->readData();

 } 
  
 // Reset BandDepthData type object contained 
 void 
 BandDepth::
 resetBandDepthData( const bdData_Type & bdData )
 {
    this->M_bdDataPtr.reset( new bdData_Type( bdData ) );
    
    this->readData();
 }
   
 // Each proces reads data from data files.
 void 
 BandDepth::
 readData()
 {
   this->M_dataSetPtr.reset( new dataSet_Type( M_bdDataPtr->nbPz(), M_bdDataPtr->nbPts(), M_bdDataPtr->leftOffset(), M_bdDataPtr->rightOffset() ) );
   
   this->M_dataSetPtr->readCSV( M_bdDataPtr->inputFilename() );
   
 }
 
 void
 BandDepth::
 compute()
 {
   int myRank, nbThreads;
   
   const UInt MASTER = 0;
   
   const UInt nbPz 	= this->M_bdDataPtr->nbPz();
   const UInt nbPts	= this->M_bdDataPtr->nbPts();
   const UInt J	   	= this->M_bdDataPtr->J();
   const UInt verbosity = this->M_bdDataPtr->verbosity();
   
   const Real * data = this->M_dataSetPtr->getData();   
   
   MPI_Comm_size( MPI_COMM_WORLD, & nbThreads );

   MPI_Comm_rank( MPI_COMM_WORLD, & myRank );   
   
   const UInt slaveProcNbPz = static_cast< UInt >( nbPz / nbThreads );
   const UInt masterProcNbPz = static_cast< UInt >( nbPz / nbThreads ) + static_cast< UInt >( nbPz % nbThreads );
   
   UInt nbMyPz;
   
   if( myRank != MASTER )
   {
      nbMyPz = slaveProcNbPz;
   }
   if ( myRank == MASTER )
   {
      nbMyPz = masterProcNbPz;
   }
   
   std::vector< Real > myBandDepths( nbMyPz );
      
   combinationFactory_Type combinationFactory( nbPz - 1, J );
   
   combinationFactory.generateCombinations();
   
   for ( UInt iPz(0); iPz < nbMyPz; ++iPz )
   {
      if( verbosity > 2 ) printf( "Proc %d is at %d / %d patients\n", myRank, iPz+1, nbMyPz );
	  
      UInt globalPzID;
      
      if ( myRank == MASTER )
      {
	globalPzID = iPz;
      }
      else
      {
	globalPzID = masterProcNbPz  + ( myRank - 1 ) * slaveProcNbPz + iPz;
      }
            
      // IMPORTANT: I leave one patient out, so the actual N in the binomial coefficient is nbPz - 1
      //combinationFactory_Type combinationFactory( nbPz - 1, J );
	
      myBandDepths[ iPz ] = 0;
      
      Real comprisedLength(0);
      
      combinationFactory.resetPointerToHeadCombination();      
      
//    while( not( combinationFactory.hasGeneratedAll() ) )
      while( not( combinationFactory.hasTraversedAll() ) )
      {
	
	tuple_Type pzTupleIDs;

	combinationFactory.getNextCombination( pzTupleIDs );
		
	// IMPORTANT: mapping the IDs of the tuple to the IDs of the population
	// The problem is that leaving one patient out will produce a tuple in
	// the range [ 0, 1, ..., nbPz - 1], while the IDs out of the combinationFactory
	// are in the range [ 0, 1, ..., nbPz ] except for globalPzID.
	for ( UInt iJ(0); iJ < J; ++iJ )
	{	  
	   if ( pzTupleIDs[ iJ ] >= globalPzID ) 
	   {
	       ++pzTupleIDs[ iJ ];
	   }
	  
	}
	
 	bool flagMaxCurr, flagMinCurr, flagMaxPrev, flagMinPrev;
 	Real envMaxPrev, envMinPrev, envMaxCurr, envMinCurr;
	
	std::vector< Real > currentValues( J );
			      
	for ( UInt iJ(0); iJ < J; ++iJ )
	{		
	  currentValues[ iJ ] = data[ pzTupleIDs[ iJ ] * nbPts + 0 ];
	} 

	envMaxPrev =  *( std::max_element( currentValues.begin(), currentValues.end() ) );
	envMinPrev =  *( std::min_element( currentValues.begin(), currentValues.end() ) );
	
	flagMaxPrev = data[ globalPzID * nbPts + 0 ] <= envMaxPrev;
	flagMinPrev = data[ globalPzID * nbPts + 0 ] >= envMinPrev;
	
	    for ( UInt iPt(1); iPt < nbPts; ++iPt )
	    {  
	      for ( UInt iJ(0); iJ < J; ++iJ )
	      {		
		  currentValues[ iJ ] = this->M_dataSetPtr->getData()[ pzTupleIDs[ iJ ] * nbPts + iPt ];
	      } 
	      
	      envMaxCurr = *( std::max_element( currentValues.begin(), currentValues.end() ) );
	      envMinCurr = *( std::min_element( currentValues.begin(), currentValues.end() ) );
	      
      	      flagMaxCurr = data[ globalPzID * nbPts + iPt ] <= envMaxCurr;
	      flagMinCurr = data[ globalPzID * nbPts + iPt ] >= envMinCurr;

	      if ( flagMaxCurr && flagMinCurr && flagMaxPrev && flagMinPrev )
	      {
		  comprisedLength += 1;
	      }
	      else
	      {
		  Real valueCurr = data[ globalPzID * nbPts + iPt ];
		  Real valuePrev = data[ globalPzID * nbPts + iPt - 1];

		  if ( not( flagMaxCurr ) )
		  {
		    if ( flagMaxPrev )
		    {
			comprisedLength += ( valuePrev - envMaxPrev ) / ( (envMaxCurr - envMaxPrev) - ( valueCurr - valuePrev ) );
		    } 
		  }
		  else if ( not( flagMaxPrev ) )
		  {
		      if ( flagMaxCurr )
		      {
			  comprisedLength += 1. - ( valuePrev - envMaxPrev ) / ( (envMaxCurr - envMaxPrev) - ( valueCurr - valuePrev ) );
		      }
		  }
		  
		  if( not( flagMinCurr ) )
		  {
		    if ( flagMinPrev )
		    {
			comprisedLength += ( valuePrev - envMinPrev ) / ( (envMinCurr - envMinPrev) - ( valueCurr - valuePrev ) );
		    }
		  }
		  else if ( not( flagMinPrev ) )
		  {
		      if ( flagMinCurr )
		      {
			comprisedLength += 1. - ( valuePrev - envMinPrev ) / ( (envMinCurr - envMinPrev) - ( valueCurr - valuePrev ) );
		      }
		  }
	      }
	      
	      flagMaxPrev = flagMaxCurr;
	      flagMinPrev = flagMinCurr;

	      envMaxPrev = envMaxCurr;
	      envMinPrev = envMinCurr;

	    }
	}

 	myBandDepths[ iPz ] = comprisedLength / static_cast< Real > ( ( nbPts - 1 ) * binomial( nbPz - 1, J ) );
  }	    
    
 
  // COMMUNICATING BAND DEPTHS

  this->M_BDs.resize( nbPz );
    
  for ( UInt iThread(1); iThread < nbThreads; ++iThread )
  {
      if( iThread == MASTER )
      {
	MPI_Bcast( & (this->M_BDs), masterProcNbPz, MPI_DOUBLE_PRECISION, MASTER, MPI_COMM_WORLD );    	  
      }
      else
      {
	MPI_Bcast( &(this->M_BDs) + masterProcNbPz + ( iThread - 1 ) * slaveProcNbPz, slaveProcNbPz, MPI_DOUBLE_PRECISION, iThread, MPI_COMM_WORLD );
      }
  }

  MPI_Barrier( MPI_COMM_WORLD );
    
  if ( verbosity > 2 && myRank == MASTER ) 
    printf( " All depths have been gathered\n" );   
 
   return;
  
} 

UInt binomial( const UInt & N , const UInt & K )
{    
    UInt num( 1 );
    UInt denom( 1 );
    
    for ( UInt iK(0); iK < K; ++iK )
    {
	num *= N - iK;
	denom *= iK + 1;
    }
  
   return static_cast< UInt >( num/denom );
}

   
}