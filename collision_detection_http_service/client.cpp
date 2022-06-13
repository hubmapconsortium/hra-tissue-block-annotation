#include <cpprest/http_client.h>
#include <cpprest/json.h>
#pragma comment(lib, "cpprest_2_10")

using namespace web;
using namespace web::http;
using namespace web::http::client;

#include <iostream>
using namespace std;

void display_json(json::value const & jvalue, utility::string_t const & prefix)
{
    cout << prefix << jvalue.serialize() << endl;
}

pplx::task<http_response> make_task_request(
    http_client & client,
    method mtd,
    json::value const & jvalue)
{
    return (mtd == methods::GET || mtd == methods::HEAD) ? 
    client.request(mtd, "/restdemo") : 
    client.request(mtd, "/restdemo", jvalue);
}

void make_request(http_client & client, method mtd, json::value const & jvalue)
{
    make_task_request(client, mtd, jvalue)
    .then([](http_response response)
    {
        if (response.status_code() == status_codes::OK)
        {
            return response.extract_json();
        }
        return pplx::task_from_result(json::value());
    }
    )
    .then([] (pplx::task<json::value> previousTask)
    {
        try
        {
            display_json(previousTask.get(), "R: ");
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    }
    )
    .wait();
}

json::value readJsonFile(std::string const & jsonFileName)
{
    json::value output;  // JSON read from input file

    try
    {
        // Open the file stream
        std::ifstream f(jsonFileName);
        // String stream for holding the JSON file
        std::stringstream strStream;

        // Stream file stream into string stream
        strStream << f.rdbuf();
        f.close();  // Close the filestream

        // Parse the string stream into a JSON object
        output = json::value::parse(strStream);
    }
    catch (json::json_exception excep)
    {
        throw json::json_exception("Error Parsing JSON file " + jsonFileName);
    }

    return output;
}

int main()
{

    http_client client(U("http://192.168.1.100:12345"));

    // auto getvalue = json::value::array();
    // getvalue[0] = json::value::string("one");
    // getvalue[1] = json::value::string("two");
    // getvalue[2] = json::value::string("three");

    auto getvalue = readJsonFile("../request.json");
    cout << "\nPOST (get some values)\n";
    display_json(getvalue, "S: ");
    make_request(client, methods::POST, getvalue);

    return 0;
}