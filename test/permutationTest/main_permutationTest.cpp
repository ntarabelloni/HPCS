
#include <source/matrixDistances.hpp>
#include <source/permutationTest.hpp>

#include <source/HPCSDefs.hpp>

using namespace std;

using namespace HPCS;

typedef DataSet dataSet_Type;
typedef boost::shared_ptr< dataSet_Type > dataSetPtr_Type;

typedef MatrixDistanceBase::matrix_Type matrix_Type;
typedef MatrixDistanceBase::matrixPtr_Type matrixPtr_Type;

typedef MatrixDistanceFactory distFactory_Type;
typedef boost::shared_ptr< MatrixDistanceBase > distPtr_Type;

typedef PermutationTest permutationTest_Type;

int main( int argc, char * argv[] )
{
  MPI_Init( & argc, & argv );
  
    int myRank, nbThreads;
    
    MPI_Comm_size( MPI_COMM_WORLD, & nbThreads );
    
    MPI_Comm_rank( MPI_COMM_WORLD,  & myRank );
  
    GetPot command_line( argc, argv );
      
    const string data_file_name = command_line.follow( "data", 2, "-f", "--file" );
    
    GetPot dataFile( data_file_name.data() );

    const std::string baseName( "PERMUTATION_TEST" );
    
    const UInt nIter = dataFile( ( baseName + "/NIter" ).data(), 100 );
    
    const UInt nbSamples1 = dataFile( ( baseName + "/nbSamples1" ).data(), 1 );
    const UInt nbSubSamples1 = dataFile( ( baseName + "/nbSubSamples1" ).data(), static_cast< int >( nbSamples1 ) );
    const UInt seed1 = dataFile( ( baseName + "/seed1" ).data(), 1 );
    
    const UInt nbSamples2 = dataFile( ( baseName + "/nbSamples2" ).data(), 1 );
    const UInt nbSubSamples2 = dataFile( ( baseName + "/nbSubSamples2" ).data(), static_cast< int >( nbSamples2 ) );
    const UInt seed2 = dataFile( ( baseName + "/seed2" ).data(), 1 );
    
    const UInt nbPts = dataFile( ( baseName + "/nbPts" ).data(), 1 );
    
    const std::string distanceID = dataFile( ( baseName + "/distance").data(), "Frobenius" );
   
    const std::string inputFile1 = dataFile( ( baseName + "/inputFile1" ).data(), "dataSet1.dat" );
    
    const std::string inputFile2 = dataFile( ( baseName + "/inputFile2" ).data(), "dataSet2.dat" );
  
    std::cout << " *** READING DATA " << std::endl;
    
    dataSetPtr_Type dataSetPtr1( new dataSet_Type( nbSamples1, nbPts ) );
    dataSetPtr_Type dataSetPtr2( new dataSet_Type( nbSamples2, nbPts ) );    
    
    std::cout << " ### Dataset 1 " << std::endl;
    
    dataSetPtr1->readData( inputFile1 );
//     dataSetPtr1->showMe();
    
    std::cout << std::endl << std::endl;
    std::cout << " ### Dataset 2 " << std::endl;
    
    dataSetPtr2->readData( inputFile2 );
//     dataSetPtr2->showMe();

    std::cout << std::endl << std::endl;
    
    distFactory_Type distFactory;
    
    distPtr_Type distPtr( distFactory.create( distanceID ) );
    
    permutationTest_Type Test( dataSetPtr1, dataSetPtr2, distPtr, nIter );
    
    if ( nbSubSamples1 != nbSamples1 )
    {
	Test.setFirstSubDataSet( nbSubSamples1, seed1 );
    }
    
    if ( nbSubSamples2 != nbSamples2 )
    {
	Test.setSecondSubDataSet( nbSubSamples2, seed2 );
    }
    
    Test.apply();
    
    std::cout << " THE P-Value is " << Test.pValue() << std::endl;
    
   
    std::cout << "##################################################" << std::endl;
    
  MPI_Finalize();
  
    return EXIT_SUCCESS;
}