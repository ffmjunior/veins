#include <sstream>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[]) {
    // "simple_map.rou.xml"
    string input_file = argv[1];
    // "simple_map_fix.rou.xml"
    string output_file = argv[2];

    printf("Input File: %s\n", input_file.c_str());
    printf("Output File: %s\n", output_file.c_str());

    freopen(input_file.c_str(), "r", stdin);
    // freopen("simple_map.rou.xml", "r", stdin);

    string line;
    ofstream output;
    output.open(output_file);
    //output.open("simple_map_fix.rou.xml");

    string origin = "<routes>\n\t";
    string vtype_original = "<vType id=\"Car\" maxSpeed=\"14.0\"/>\n";
    string vtype =
            "<vType id=\"vtype0\" accel=\"2.6\" decel=\"4.5\" sigma=\"0.5\" length=\"2.5\" minGap=\"2.5\" maxSpeed=\"14\" color=\"1,1,0\"/>\n";

    output << origin + vtype;

    int count = 0;

    while (getline(cin, line)) {
        if (line.compare(0, 15, "        <route ") == 0) {
            string route = "\t<route id=\"";
            route += "route" + to_string(count);
            route += "\"";
            route += line.substr(14) + "\n"; // creates a route;
            output << route;
            string flow = "\t<flow id=\"";
            flow += "flow" + to_string(count);
            flow += "\"";
            flow += " type=\"vtype0\" route=";
            flow += "\"";
            flow += "route" + to_string(count);
            flow += "\" begin=\"0\" period=\"3\" number=\"10\"/>\n";
            output << flow;
            count = count + 1;

        }

    }

    output << "</routes>";

    return 0;
}

