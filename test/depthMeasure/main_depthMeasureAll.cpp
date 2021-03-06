

#include <source/HPCSDefs.hpp>

#include <source/depthMeasure.hpp>
#include <source/depthMeasureFactory.hpp>
#include <source/bandDepthData.hpp>

using namespace std;
using namespace HPCS;

typedef DepthMeasureBase< All > dmBase_Type;
typedef boost::shared_ptr< dmBase_Type > dmBasePtr_Type;
typedef DMFactory< All > dmFactory_Type;
typedef BandDepthData bdData_Type;
typedef boost::shared_ptr< bdData_Type > bdDataPtr_Type;

int main( int argc, char * argv[] )
{ 
  MPI_Init( &argc, &argv );
  
   int nbThreads, myRank;
   
   MPI_Comm_rank( MPI_COMM_WORLD, & myRank );
   
   MPI_Comm_size( MPI_COMM_WORLD, & nbThreads );
   
   if ( myRank == MASTER )
   {
      printf( "=======================================\n" );
   }
   
   // INPUT OF DATA FROM FILEANAMES
   
   if ( myRank == MASTER )
   {
      printf( " *** DATA SETUP *** \n" );
   }
   
   GetPot command_line( argc, argv );
      
   const string data_file_name = command_line.follow( "data", 2, "-f", "--file" );
     
   GetPot dataFile( data_file_name.data() );
   
   bdDataPtr_Type bdDataPtr( new bdData_Type( dataFile, "BDALL") );
   
   dmFactory_Type factory;
   
   dmBasePtr_Type dmPtr( factory.create( bdDataPtr->J() ) );
   
   dmPtr->setBandDepthData( bdDataPtr );

   if ( myRank == MASTER )
   {
      printf( " *** COMPUTATION OF DEPTHS *** \n" );
   }
   
   dmPtr->computeDepths();
   
   dmPtr->computeRanks();

   dmPtr->writeDepths();

   if ( myRank == MASTER )
   {
      printf( " *** RANKS INDUCED BY DEPTHS *** \n" );
   }
   
   dmPtr->writeRanks();
   
   if ( myRank == MASTER )
   {
      printf( "=======================================\n" );
   }
   
 MPI_Finalize();
  
  return EXIT_SUCCESS;
  
}