#include "ReadWriteFile.h"

using namespace std;

string float_to_string(float Fin)
{
    stringstream buff;
    string out;

    buff << Fin;
    buff >> out;

    return out;
}

float string_to_float(string Sin)
{
    stringstream buff;
    float out;

    buff << Sin;
    buff >> out;

    return out;
}


void InfoReadWrite::save_txt(string filename_out, string out)
{

    ofstream myfile (filename_out.c_str());

	if (myfile.is_open())
	{
        myfile <<out;
        myfile.close();
        //cout<<"File "<< filename_out<<" written"<<endl;
	}else{
	    cout<<"Problem with file I/O: "<<filename_out<<endl;
	}

}

string InfoReadWrite::load_txt(string filename_in)
{

    string line;
    string all_lines="";
    ifstream myfile (filename_in.c_str());

    if (myfile.is_open())
    {

        while (! myfile.eof())
        {
            getline (myfile,line);
            all_lines=all_lines +line +"\n";
        }
        myfile.close();
    }else{
    cout<<"File not found: "<<filename_in<<endl;
    }

    return all_lines;
}

string InfoReadWrite::find_subs_between(string mess_in,string left, string right)
{
	//extracting posx:
	int pos;
	string Spos="";

	int loc_1 = mess_in.find( left, 0 );

   	if( loc_1 != string::npos ){
		int loc_2 = mess_in.find( right, loc_1+1 );

		if( loc_2 != string::npos ){
			Spos = mess_in.substr(loc_1 + left.size(), loc_2-loc_1 - left.size());
		}else{
		    cout<<"Can't find right element."<<endl;
		    }
	}else{
        cout<<"Can't find left element."<<endl;
    }

    return Spos;
}

InfoReadWrite::InfoReadWrite()
{
    UploadB=0.12f;
    DownloadB=12.0f;
}

//!Read the class from a file
void InfoReadWrite::Read(string filename)
{
    string Sin = load_txt(filename);

    DownloadB = string_to_float( find_subs_between(Sin, "DownloadToday=", "\n") );
    UploadB = string_to_float( find_subs_between(Sin, "UploadToday=", "\n") );
}

//!Write the class to a file
void InfoReadWrite::Write(string filename)
{
    string out="";
    out = out + "DownloadToday=" + float_to_string(DownloadB) + "\n";
    out = out + "UploadToday=" + float_to_string(UploadB) + "\n";
    save_txt(filename, out);
}

vector<float> InfoReadWrite::getData()
{
    vector<float> out;
    out.push_back(DownloadB);
    out.push_back(UploadB);
    return out;
}

void InfoReadWrite::setData(vector<float> in)
{
    if(in.size() == 2)
    {
        DownloadB = in[0];
        UploadB = in[1];
    }
    else
    cout<<"Invalid data, won't write the file."<<endl;
}
