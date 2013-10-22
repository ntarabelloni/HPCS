
#include <source/matrixDistances.hpp>
#include <source/permutationTest.hpp>

#include <source/HPCSDefs.hpp>

using namespace std;

using namespace HPCS;

typedef DataSet dataSet_Type;
typedef boost::shared_ptr< dataSet_Type > dataSetPtr_Type;

typedef MatrixDistanceBase::matrix_Type matrix_Type;
typedef MatrixDistanceBase::matrixPtr_Type matrixPtr_Type;

typedef FrobeniusDistance dist_Type;
typedef boost::shared_ptr< dist_Type > distPtr_Type;

typedef PermutationTest permutationTest_Type;

int main( int argc, char * argv[] )
{
    GetPot command_line( argc, argv );
      
    const string data_file_name = command_line.follow( "data", 2, "-f", "--file" );
    
    GetPot dataFile( data_file_name.data() );

    const std::string baseName( "PERMUTATION_TEST" );
    
    const UInt nIter = dataFile( ( baseName + "/NIter" ).data(), 100 );
    
    const UInt nbSamples = dataFile( ( baseName + "/nbSamples" ).data(), 1 );
    
    const UInt nbPts = dataFile( ( baseName + "/nbPts" ).data(), 1 );
   
    const std::string inputFile1 = dataFile( ( baseName + "/inputFile1" ).data(), "dataSet1.dat" );
    
    const std::string inputFile2 = dataFile( ( baseName + "/inputFile2" ).data(), "dataSet2.dat" );
  
    std::cout << " *** READING DATA " << std::endl;
    
    dataSetPtr_Type dataSetPtr1( new dataSet_Type( nbSamples, nbPts ) );
    dataSetPtr_Type dataSetPtr2( new dataSet_Type( nbSamples, nbPts ) );    
    
    std::cout << " ### Dataset 1 " << std::endl;
    
    dataSetPtr1->readData( inputFile1 );
    dataSetPtr1->showMe();
    
    std::cout << std::endl << std::endl;
    std::cout << " ### Dataset 2 " << std::endl;
    
    dataSetPtr2->readData( inputFile2 );
    dataSetPtr2->showMe();

    std::cout << std::endl << std::endl;
    
    distPtr_Type distPtr( new dist_Type() );
    
    permutationTest_Type Test( dataSetPtr1, dataSetPtr2, distPtr, nIter );
    
    Test.apply();
    
    std::cout << " THE P-Value is " << Test.pValue() << std::endl;
    
   
    std::cout << "##################################################" << std::endl;
    
  
    return EXIT_SUCCESS;
}