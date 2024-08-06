#include "json_utils.h"


void load_organ_transformation(const std::string &json_file_path, std::unordered_map<std::string, Placement> &mapping_placement)
{
    json::value json_output = read_json_file(json_file_path);

    try 
    {
        auto json_output_object = json_output.as_object();
        auto placement = json_output_object.at("placementPatches");
        auto placement_object = placement.as_object();
        
        for (auto it=placement_object.cbegin(); it!=placement_object.cend(); ++it)
        {
            // auto &source = (it->first).as_string();
            auto &propertyValue = it->second;

            std::string source = propertyValue.at("source").as_string();
            std::string target = propertyValue.at("target").as_string();
            double x_scaling = propertyValue.at("x_scaling").as_double();
            double y_scaling = propertyValue.at("y_scaling").as_double();
            double z_scaling = propertyValue.at("z_scaling").as_double();
            double x_translation = propertyValue.at("x_translation").as_double();
            double y_translation = propertyValue.at("y_translation").as_double();
            double z_translation = propertyValue.at("z_translation").as_double();
            double x_rotation = propertyValue.at("x_rotation").as_double();
            double y_rotation = propertyValue.at("y_rotation").as_double();
            double z_rotation = propertyValue.at("z_rotation").as_double();
            

            mapping_placement[source] = Placement(source, target, 
                                                            x_scaling, y_scaling, z_scaling,
                                                            x_translation, y_translation, z_translation,
                                                            x_rotation, y_rotation, z_rotation);
            
        }
        adjust_placement_target_ancestor(mapping_placement);
        // for (int i = 0; i < placement.size(); i++) 
        // {
        //     auto item = placement[i];
        //     auto itemObj = item.as_object();
        //     for (auto iterInner = itemObj.cbegin(); iterInner != itemObj.cend(); ++iterInner)
        //     {
        //         auto &propertyValue = iterInner->second;
        //         std::string source = propertyValue.at("source").as_string();
        //         std::string target = propertyValue.at("target").as_string();
        //         double x_scaling = propertyValue.at("x_scaling").as_double();
        //         double y_scaling = propertyValue.at("y_scaling").as_double();
        //         double z_scaling = propertyValue.at("z_scaling").as_double();
        //         double x_translation = propertyValue.at("x_translation").as_double();
        //         double y_translation = propertyValue.at("y_translation").as_double();
        //         double z_translation = propertyValue.at("z_transaltion").as_double();
        //         double x_rotation = propertyValue.at("x_rotation").as_double();
        //         double y_rotation = propertyValue.at("y_rotation").as_double();
        //         double z_rotation = propertyValue.at("z_rotation").as_double();

        //         mapping_placement[source] = Placement(source, target, x_scaling, y_scaling, z_scaling, x_translation, y_translation, z_translation, x_rotation, y_rotation, z_rotation);

        //     }
        // }



    }
    catch(...) 
    {
        std::cout << "catch exception in parse " + json_file_path << std::endl;
    }


}

json::value read_json_file(std::string const & json_file_path)
{
    json::value output;  // JSON read from input file

    try
    {
        // Open the file stream
        std::ifstream f(json_file_path);
        // String stream for holding the JSON file
        std::stringstream strStream;

        // Stream file stream into string stream
        strStream << f.rdbuf();

        // std::cout << strStream.str() << std::endl;
        f.close();  // Close the filestream

        // Parse the string stream into a JSON object
        output = json::value::parse(strStream);
    }
    catch (json::json_exception excep)
    {
        throw json::json_exception("Error Parsing JSON file " + json_file_path);
    }

    return output;
}

void adjust_placement_target_ancestor(std::unordered_map<std::string, Placement> &mapping_placement)
{
    for (auto &item: mapping_placement)
    {
        auto source = item.first;
        auto &placement = item.second;        
        auto target = placement.target;
        auto x_scaling = placement.x_scaling;
        auto y_scaling = placement.y_scaling;
        auto z_scaling = placement.z_scaling;


        while (mapping_placement.find(target) != mapping_placement.end())
        {
            auto &cur_placement = mapping_placement[target];
            target = cur_placement.target;
            x_scaling *= cur_placement.x_scaling;
            y_scaling *= cur_placement.y_scaling;
            z_scaling *= cur_placement.z_scaling;
        }

        placement.target = target;
        placement.x_scaling = x_scaling;
        placement.y_scaling = y_scaling;
        placement.z_scaling = z_scaling;
    }

    // print_mapping_placement(mapping_placement);

}


void adjust_placement_target_ancestor_grlc(std::unordered_map<std::string, Placement> &mapping_placement)
{

    for (auto &item: mapping_placement)
    {
        auto source = item.first;
        auto &placement = item.second;        
        auto target = placement.target;
        auto x_scaling = placement.x_scaling;
        auto y_scaling = placement.y_scaling;
        auto z_scaling = placement.z_scaling;


        // while (source != target && mapping_placement.find(target) != mapping_placement.end())
        // {
        //     auto &cur_placement = mapping_placement[target];
        //     //record the source to avoid https://purl.humanatlas.io/ref-organ/brain-female#primary
        //     source = target;
        //     target = cur_placement.target;
        //     x_scaling *= cur_placement.x_scaling;
        //     y_scaling *= cur_placement.y_scaling;
        //     z_scaling *= cur_placement.z_scaling;
        // }

        // // in reference-organ-grlc.csv, the target will finally point to https://purl.humanatlas.io/ref-organ/brain-female#primary, so we need to use source to indicate
        // placement.target = source;
        // placement.x_scaling = x_scaling;
        // placement.y_scaling = y_scaling;
        // placement.z_scaling = z_scaling;

        while (mapping_placement.find(target) != mapping_placement.end())
        {
            auto &cur_placement = mapping_placement[target];
            target = cur_placement.target;
            x_scaling *= cur_placement.x_scaling;
            y_scaling *= cur_placement.y_scaling;
            z_scaling *= cur_placement.z_scaling;
        }

        placement.target = target;
        placement.x_scaling = x_scaling;
        placement.y_scaling = y_scaling;
        placement.z_scaling = z_scaling;
    }

}


void print_mapping_placement(std::unordered_map<std::string, Placement> &mapping_placement)
{
    for (auto &item: mapping_placement)
    {
        auto source = item.first;
        auto &placement = item.second;

        std::cout << "source: " << placement.source << " target: " << placement.target
        << " x_scaling: " << placement.x_scaling << " y_scaling: " << placement.y_scaling << " z_scaling: " << placement.z_scaling << std::endl;     
    }
}


void load_organ_transformation_grlc(const std::string &reference_organ_grlc_file, std::unordered_map<std::string, Placement> &mapping_placement)
{

    std::ifstream reference_organ_file(reference_organ_grlc_file);

    if (!reference_organ_file.is_open()) throw std::runtime_error("could not open reference_organ_grlc.csv!");

    std::string line;
    if (reference_organ_file.good())
    {
        // skip header
        std::getline(reference_organ_file, line);
    }

    std::vector<std::string> row;
    std::string word;
    while (std::getline(reference_organ_file, line))
    {
        row.clear();
        std::stringstream ss(line);
        while (std::getline(ss, word, ','))
        {
            row.push_back(word);
        }

        auto source = row[0];
        auto target = row[1];
        auto x_scaling = std::stod(row[2]);
        auto y_scaling = std::stod(row[3]);
        auto z_scaling = std::stod(row[4]);
        auto x_rotation = std::stod(row[5]);
        auto y_rotation = std::stod(row[6]);
        auto z_rotation = std::stod(row[7]);
        auto x_translation = std::stod(row[8]);
        auto y_translation = std::stod(row[9]);
        auto z_translation = std::stod(row[10]);


        mapping_placement[source] = Placement(source, target, 
                                                            x_scaling, y_scaling, z_scaling,
                                                            x_translation, y_translation, z_translation,
                                                            x_rotation, y_rotation, z_rotation);
    }        
    adjust_placement_target_ancestor_grlc(mapping_placement);
}

