#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <cassert>
#include <boost/regex.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include "filecurve.h"
#include "fileJSON.h"

using boost::property_tree::ptree;

template <typename T>
std::vector<T> as_vector(ptree const& pt, ptree::key_type const& key)
{
    std::vector<T> r;
    for (auto& item : pt.get_child(key))
        r.push_back(item.second.get_value<T>());
    return r;
}


void fileJSON::findAndReplaceAll(std::string & data, std::string toSearch, std::string replaceStr)
{
    // Get the first occurrence
    size_t pos = data.find(toSearch);
    // Repeat till end is reached
    while( pos != std::string::npos)
    {
        // Replace this occurrence of Sub String
        data.replace(pos, toSearch.size(), replaceStr);
        // Get the next occurrence from the current position
        pos =data.find(toSearch, pos + replaceStr.size());
    }
}

fileJSON::fileJSON( string filename )
{
    try {

        namespace pt = boost::property_tree;

        pt::ptree    root;
        stringstream ss;

        ostringstream expression;

        read_json( filename, root );

        for ( auto &curve : root.get_child("outside") ) {

            expression << '{';

            for ( auto &vertex : curve.second ) {
                expression << vertex.second.get_value<float>() << ',';
            }

            expression << "},";
        }

        string cleanedExpression{ expression.str() };

        findAndReplaceAll( cleanedExpression, ",}", "}" );

        cleanedExpression = cleanedExpression.substr( 0, cleanedExpression.length() - 1 );

        curveDefinitions.push_back( make_shared< curveExpression >( cleanedExpression ) );
   }
    catch (exception const &e) {
        cerr << e.what() << endl;
    }
}
