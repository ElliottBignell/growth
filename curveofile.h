///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///
///The meshfile class is the base class for classes writing meshes
///as output files
///

#include <iostream>
#include <iomanip>
#include <fstream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include "boost/tuple/tuple.hpp"

using namespace std;
using namespace boost::numeric::ublas;

///The meshfile class is the base class for mesh files
///!The meshfile class is the base class for classes writing meshes
///as output files. To add a new mesh file type it is necessary
///to derive a specialisation of meshfile and instantiate it in place
///of the existing specialisations.
class meshfile
{
public:
    typedef std::pair< matrix< int >, unsigned long > index;
    typedef matrix< float  > triangle;
    typedef matrix< double > normal;

protected:
    string filename;
    ofstream vectors;
    ofstream faces;
    ofstream normals;
    unsigned int vectorCnt;
    unsigned int pointCnt;
    unsigned int textureCnt;
    unsigned int normalCnt;

    meshfile( const string& name ) 
        : filename( name )
        , vectors( "/tmp/vectors.txt", std::ofstream::out | std::ofstream::app )
        , faces( "/tmp/faces.txt", std::ofstream::out | std::ofstream::app )
        , normals( "/tmp/normals.txt", std::ofstream::out | std::ofstream::app )
        , vectorCnt( 0 )
        , pointCnt( 0 )
        , textureCnt( 3 )
        , normalCnt( 0 )
    {
        //faces << "< 0, 0, 0>," << endl;
        //vectors << "< 0, 0, 0>," << endl;
        //vectorCnt++;
        //pointCnt++;
    }

    ~meshfile() 
    { 
        std::cout << "Points:    " << vectorCnt << std::endl;
        std::cout << "Triangles: " << pointCnt  << std::endl;
    }

    virtual void write( ifstream&, ifstream&, ifstream& ) = 0;

public:
    void close()
    {
        vectors.close();
        faces.close();
        normals.close();

        ifstream ivectors( "/tmp/vectors.txt" );
        ifstream ifaces( "/tmp/faces.txt" );
        ifstream inormals( "/tmp/normals.txt" );

        write( ivectors, ifaces, inormals );

        ivectors.close();
        ifaces.close();
        inormals.close();

        remove( "/tmp/vectors.txt" );
        remove( "/tmp/faces.txt" );
        remove( "/tmp/normals.txt" );
    }

    virtual void out( const index indices ) = 0;
    virtual void out( const triangle& tri ) = 0;
    virtual void out( const normal&  norm ) = 0;

    virtual meshfile& operator<<(       index indices ) { out( indices ); return *this; }
    virtual meshfile& operator<<( const normal&  norm ) { out( norm );    return *this; }
    virtual meshfile& operator<<(       triangle& tri ) { out( tri );     return *this; }
};

///The meshnull class is a null pattern for mesh file output
///!This file serves as a null pattern so that the mesh file output
///can be dealt with by reference. To catch bugs, assertions can be 
///included in the public methods. 
class meshnull : public meshfile
{
    static string noname;

public:
    meshnull() : meshfile( noname ) {}

    virtual void write( ifstream&, ifstream&, ifstream& ) {}
    virtual void out( const index indices ) {}
    virtual void out( const triangle& tri ) {}
    virtual void out( const normal&  norm ) {}
};

string meshnull::noname;

///The meshpov class outputs the mesh in a POVRay-compliant (*.pov) format
class meshpov : public meshfile
{
public:
    meshpov( string& name ) : meshfile( name ) {}

    virtual void write( ifstream& ivectors, ifstream& ifaces, ifstream& inormals )
    {
        ofstream combined_file( filename.c_str() ) ;

        combined_file 
            << "#include \"textures.pov\"" << endl
            << endl
            << "#declare SHELL = meshpov {" << endl
            << "vertex_vectors {" << endl
            << "\t" << vectorCnt << ","
            << ivectors.rdbuf()  << endl
            << "}" << endl
            << "normal_vectors {" << endl
            << "\t" << normalCnt << ","
            << inormals.rdbuf()  << endl
            << "}" << endl
            << "texture_list {" << endl
            << "\t" << textureCnt << "," << endl
            << "\ttexture {  defTexture1 }," << endl
            << "\ttexture {  defTexture2 }," << endl
            << "\ttexture {  insideTexture_ }" << endl
            << "}" << endl
            << "face_indices {" << endl
            << "\t" << pointCnt << ","
            << ifaces.rdbuf() << endl
            << "}" << endl
            << "}" << endl;

            //<< "pigment { color rgb< 0.9, 0.9, 0.7 > } "
            //<< "finish { reflection .3 phong 2} "

        combined_file.close();

    }

    virtual void out( const index indices )
    {
        if ( 
               indices.first( 0, 0 ) == indices.first( 0, 1 ) ||
               indices.first( 0, 0 ) == indices.first( 0, 2 ) ||
               indices.first( 0, 1 ) == indices.first( 0, 2 )
           ) 
        {
            return;
        }

        faces << ( pointCnt++ > 0 ? "," : "" ) << endl 
              << "\t<"
              << indices.first( 0, 0 ) << ","
              << indices.first( 0, 1 ) << ","
              << indices.first( 0, 2 )
              << ">, " << indices.second;
    } 

    virtual void out( const triangle& tri )
    {
        vectors << ( vectorCnt++ > 0 ? "," : "" ) << endl
                << "\t<"
                << tri( 0, 0 ) << ","
                << tri( 0, 1 ) << ","
                << tri( 0, 2 )
                << ">";
    } 

    virtual void out( const normal& norm )
    {
        normals << ( normalCnt++ > 0 ? "," : "" ) << endl
                << "\t<"
                << norm( 0, 0 ) << ","
                << norm( 0, 1 ) << ","
                << norm( 0, 2 )
                << ">";
    } 
};

///The meshpov class outputs the mesh in a Blender-compliant (*.x3d) format
class meshx3d : public meshfile
{
    static const unsigned int precision = 5;
    static constexpr double tens = pow( precision, precision );

    std::string colours;

public:
    meshx3d( string& name ) : meshfile( name ) {}
    ~meshx3d()
    {
        //std::cout << "Colours: " << colours.size() / 2 << std::endl;
    }

    virtual void write( ifstream& ivectors, ifstream& ifaces, ifstream& inormals )
    {
        ofstream combined_file( filename.c_str() ) ;

        combined_file 
            << "<X3D>" << endl
            << "<Scene>" << endl
            << "<Shape>" << endl
            << endl
            << "\t<IndexedFaceSet solid=\"false\" colorPerVertex=\"true\"" << endl
            << "\t\tcoordIndex=\""
            << ifaces.rdbuf() << endl
            //<< "\t\t\"" << endl
            //<< "\t\tcolorIndex=\"" << endl
            //<< colours  << endl
            << "\t\t\" >" << endl
            << "\t<Coordinate" << endl
            << "\t\tpoint=\"" << endl
            << ivectors.rdbuf()
            << "\t\t\" />" << endl
            << "\t\t<Color color=\"" << endl
            << colours  << endl
            << "1 0 0" << endl
            << "\t\t\" />" << endl
            << "\t</IndexedFaceSet>" << endl
            << endl
            << "\t<Appearance>" << endl
            //<< "\t\t<Material diffuseColor='1 0 0' />" << endl
            << "\t\t<Material ambientIntensity='0.25' diffuseColor='0.748014 0.62085 0.0' shininess='0.939394' specularColor='0.860606 0.860606 0.860599' transparency='0.34749'/> " << endl
            //<< "\t\t<Material diffuseColor='1 0 0' specularColor='0.823529 0.705882 0.54902'/> " << endl
            << "\t</Appearance>" << endl
            << endl
            << "</Shape>" << endl
            << "</Scene>" << endl
            << "</X3D>" << endl;

            //<< "pigment { color rgb< 0.9, 0.9, 0.7 > } "
            //<< "finish { reflection .3 phong 2} "

        combined_file.close();
    }

    bool equal( auto lhs, auto rhs )
    {
        return ( trunc( tens * lhs ) == trunc( tens * rhs ) );
    }

    virtual void out( const index indices )
    {
        if ( 
               indices.first( 0, 0 ) == indices.first( 0, 1 ) ||
               indices.first( 0, 0 ) == indices.first( 0, 2 ) ||
               indices.first( 0, 1 ) == indices.first( 0, 2 )
           ) 
        {
            return;
        }

        faces << "\n\t\t\t" << indices.first( 0, 0 )
              << " "        << indices.first( 0, 1 )
              << " "        << indices.first( 0, 2 )
              << " -1";

        float blue   = static_cast<float>( ( indices.second & 0xFF0000 ) > 16 );
        float green  = static_cast<float>( ( indices.second & 0x00FF00 ) >  8 );
        float red    = static_cast<float>( ( indices.second & 0x0000FF ) >  0 );

        std::string col = std::to_string( blue ) + " " + std::to_string( green ) + " " +  std::to_string( red ) + "\n";

        colours.append( col );

        vectorCnt++;
    } 

    virtual void out( const triangle& tri )
    {
        using namespace std;

        vectors << "\t\t\t" 
                << ( ( pointCnt++ > 0 ) ? ',' : ' ' )
                        << fixed << setprecision( 5 ) << tri( 0, 0 )
                << " "  << fixed << setprecision( 5 ) << tri( 0, 1 )
                << " "  << fixed << setprecision( 5 ) << tri( 0, 2 )
                << "\n";
    } 

    virtual void out( const normal& norm )
    {
        normals << "\t\t\t" << 1
                << ( ( normalCnt++ > 0 ) ? ',' : ' ' )
                << " "      << 0
                << " "      << 0
                << "\n";
    } 
};
