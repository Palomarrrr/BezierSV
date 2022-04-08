#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include <matplotlib-cpp/matplotlibcpp.h>

using std::cout, std::endl;

namespace plt = matplotlibcpp;

int main()
{
	std::fstream fp;
	fp.open("SVPlot.txt", std::ios_base::in);
	std::vector<int> xPoints;
	std::vector<float> yPoints;
	std::string buff;

	bool flipper = 0;

	if(fp.is_open())
	{
		char c;
		while(!fp.eof())
		{
	
			fp.get(c);
			if( c == ',' && buff.length() > 0) 
			{
				flipper = 1;
				for(int i = 0; i < buff.length(); i++)
				{
					cout << buff[i];
				}
				//cout << __LINE__ << endl;
				xPoints.push_back(std::stoi(buff));
				buff = {};
			}
			else if ( c == '\n' && buff.length() > 0)
			{
				flipper = 0;
				for(int i = 0; i < buff.length(); i++)
				{
					cout << buff[i];
				}
				//cout << __LINE__ << endl;
				yPoints.push_back(std::stof(buff));
				buff = {};
			}
			else 
			{	
				buff += c;
			}
		}	
		
		fp.close();
		
		if(fp.is_open())
		{
			cout << "Could not successfully close file" << endl;
		}
		
		plt::plot(xPoints, yPoints);
		
		plt::save("SVPlot.png");
	}
}


