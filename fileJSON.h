///
///@file
///@author  Elliott Bignell <elliott.bignell@gmail.com>
///@version 1.0
///

#ifndef __FILEJSON_H__
#define __FILEJSON_H__

using namespace std;

class curveExpression;

class fileJSON
{
    std::vector< shared_ptr< curveExpression > > curveDefinitions;

public:
    fileJSON( std::string );

    const std::vector< shared_ptr< curveExpression > >& getDefinitions() const { return curveDefinitions; }

private:
    void findAndReplaceAll(std::string&, std::string, std::string );
};

#endif // __FILEJSON_H__
