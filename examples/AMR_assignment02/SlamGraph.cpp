#include "SlamGraph.h"
#include "MatrixIO.h"
#include <map>

bool readSlamGraph(const std::string& nodeFile, const std::string& edgeFile, SlamGraph& graph, int maxNodes)
{
	graph.poses.clear();
	graph.constraints.clear();

	std::map<int,int> idmap;

	// read nodes
	// VERTEX2 id pose.x pose.y pose.theta
	{
		std::ifstream file(nodeFile.c_str());
		if (!file.good())
			return false;
		while (!file.eof())
		{
			std::string line;
			std::getline(file,line,'\n');
			std::vector<std::string> words=explode(line," ");

			if (words.size()!=5)
				continue;
			if (words[0]!="VERTEX2")
				return false;

			Vector3d p(convertToT<double>(words[2]),convertToT<double>(words[3]),convertToT<double>(words[4]));
			graph.poses.push_back(p);
			idmap[convertToT<int>(words[1])]=graph.poses.size()-1;

			if ((maxNodes!=-1)&&(idmap.size()>maxNodes))
				break;
		}
		file.close();
	}

	// read edges
	// wrong:
	// EDGE2 idFrom idTo mean.x mean.y mean.theta inf.xx inf.xy inf.yy inf.xt inf.yt inf.tt
	// correct:
	// EDGE2 idFrom idTo mean.x mean.y mean.theta inf.xx inf.xy inf.yy inf.tt inf.xt inf.yt
	{
		std::ifstream file(edgeFile.c_str());
		if (!file.good())
			return false;
		while (!file.eof())
		{
			std::string line;
			std::getline(file,line,'\n');
			std::vector<std::string> words=explode(line," ");

			if (words.size()!=12)
				continue;
			if (words[0]!="EDGE2")
				return false;

			if ((idmap.find(convertToT<int>(words[1]))==idmap.end())||(idmap.find(convertToT<int>(words[2]))==idmap.end()))
				continue;

			Constraint c;
			c.i=idmap[convertToT<int>(words[1])];
			c.j=idmap[convertToT<int>(words[2])];
			c.zij=Vector3d(convertToT<double>(words[3]),convertToT<double>(words[4]),convertToT<double>(words[5]));

			Matrix3d& inf=c.Sij_inv;
			inf(0,0)=convertToT<double>(words[6]);
			inf(1,0)=inf(0,1)=convertToT<double>(words[7]);
			inf(1,1)=convertToT<double>(words[8]);
			inf(2,0)=inf(0,2)=convertToT<double>(words[10]);
			inf(2,1)=inf(1,2)=convertToT<double>(words[11]);
			inf(2,2)=convertToT<double>(words[9]);

			graph.constraints.push_back(c);
		}
		file.close();
	}

	return true;
}
