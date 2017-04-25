///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///
///@section LICENSE
///
///This program is free software; you can redistribute it and/or
///modify it under the terms of the GNU General Public License as
///published by the Free Software Foundation; either version 2 of
///the License, or (at your option) any later version.
///
///This program is distributed in the hope that it will be useful, but
///WITHOUT ANY WARRANTY; without even the implied warranty of
///MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
///General Public License for more details at
///https://www.gnu.org/copyleft/gpl.html
///
///@section DESCRIPTION
///
///The meshfile class is the base class for classes writing meshes
///as output files
///

#include <iostream>
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
    typedef std::pair< matrix< int >, unsigned int > index;
    typedef matrix< float  > triangle;
    typedef matrix< double > normal;

protected:
    string filename;
    ofstream vectors;
    ofstream faces;
    ofstream normals;
    unsigned int vectorCnt;
    unsigned int faceCnt;
    unsigned int textureCnt;
    unsigned int normalCnt;

    meshfile( const string& name ) 
        : filename( name )
        , vectors( "/tmp/vectors.txt", std::ofstream::out | std::ofstream::app )
        , faces( "/tmp/faces.txt", std::ofstream::out | std::ofstream::app )
        , normals( "/tmp/normals.txt", std::ofstream::out | std::ofstream::app )
        , vectorCnt( 0 )
        , faceCnt( 0 )
        , textureCnt( 3 )
        , normalCnt( 0 )
    {
        //faces << "< 0, 0, 0>," << endl;
        //vectors << "< 0, 0, 0>," << endl;
        //vectorCnt++;
        //faceCnt++;
    }

    ~meshfile() { }

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

    virtual void out( index indices ) = 0;
    virtual void out( triangle& tri ) = 0;
    virtual void out( normal&  norm ) = 0;

    virtual meshfile& operator<<( index indices ) { out( indices ); return *this; }
    virtual meshfile& operator<<( normal&  norm ) { out( norm );    return *this; }
    virtual meshfile& operator<<( triangle& tri ) { out( tri );     return *this; }
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
    virtual void out( index indices ) {}
    virtual void out( triangle& tri ) {}
    virtual void out( normal&  norm ) {}
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
            << "\t" << faceCnt << ","
            << ifaces.rdbuf() << endl
            << "}" << endl
            << "}" << endl;

            //<< "pigment { color rgb< 0.9, 0.9, 0.7 > } "
            //<< "finish { reflection .3 phong 2} "

        combined_file.close();

    }

    virtual void out( index indices )
    {
        if ( 
               indices.first( 0, 0 ) == indices.first( 0, 1 ) ||
               indices.first( 0, 0 ) == indices.first( 0, 2 ) ||
               indices.first( 0, 1 ) == indices.first( 0, 2 )
           ) 
        {
            return;
        }

        faces << ( faceCnt++ > 0 ? "," : "" ) << endl 
              << "\t<"
              << indices.first( 0, 0 ) << ","
              << indices.first( 0, 1 ) << ","
              << indices.first( 0, 2 )
              << ">, " << indices.second;
    } 

    virtual void out( triangle& tri )
    {
        vectors << ( vectorCnt++ > 0 ? "," : "" ) << endl
                << "\t<"
                << tri( 0, 0 ) << ","
                << tri( 0, 1 ) << ","
                << tri( 0, 2 )
                << ">";
    } 

    virtual void out( normal& norm )
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
public:
    meshx3d( string& name ) : meshfile( name ) {}

    virtual void write( ifstream& ivectors, ifstream& ifaces, ifstream& inormals )
    {
        ofstream combined_file( filename.c_str() ) ;

        std::string colours;

        colours.reserve( faceCnt * 2 );

        for ( unsigned int n = 0; n < faceCnt; n++ ) {
            colours.append( "0 " );
        }

        combined_file 
            << "<X3D>" << endl
            << "<Scene>" << endl
            << "<Shape>" << endl
            << endl
            << "\t<IndexedFaceSet solid=\"false\" colorPerVertex=\"true\"" << endl
            << "\t\tcoordIndex=\""
            << ifaces.rdbuf() << endl
            << "\t\t\"" << endl
            << "\t\tcolorIndex=\"" << endl
            << colours  << endl
            << "\t\t\" >" << endl
            << "\t<Coordinate" << endl
            << "\t\tpoint=\"" << endl
            << ivectors.rdbuf()
            << "\t\t\" />" << endl
            << "\t\t<Color color=\"" << endl
            << "1 0 0" << endl
            << "\t\t\" />" << endl
            << "\t</IndexedFaceSet>" << endl
            << endl
            << "\t<Appearance>" << endl
            << "\t\t<Material diffuseColor='1 0 0' />" << endl
            << "\t</Appearance>" << endl
            << endl
            << "</Shape>" << endl
            << "</Scene>" << endl
            << "</X3D>" << endl;

            //<< "pigment { color rgb< 0.9, 0.9, 0.7 > } "
            //<< "finish { reflection .3 phong 2} "

        combined_file.close();


    }

    virtual void out( index indices )
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
              << " "      << indices.first( 0, 1 )
              << " "      << indices.first( 0, 2 )
              << " -1";
    } 

    virtual void out( triangle& tri )
    {
        vectors << "\t\t\t" << tri( 0, 0 )
                << " "    << tri( 0, 1 )
                << " "    << tri( 0, 2 )
                << ",\n";
        faceCnt++;
    } 

    virtual void out( normal& norm )
    {
        normals << "\t\t\t" << 1
                << " "      << 0
                << " "      << 0
                << ",\n";
    } 
};
