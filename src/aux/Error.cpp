#include "aux/Error.h"

namespace pink {
    Error::Error(const Error& other)
        : kind(other.kind), dsc(other.dsc), loc(other.loc)
    {

    }

    Error::Error(Kind k, const char* d, Location l)
        : kind(k), dsc(d), loc(l)
    {

    }

	Error::Error(Kind k, std::string d, Location l)
		: kind(k), dsc(d), loc(l)
	{
		
	}

    Error& Error::operator=(const Error& other)
    {
        kind = other.kind;
        dsc  = other.dsc;
        loc  = other.loc;
        return *this;
    }

    std::string Error::ToString(const char* txt)
    {
        std::string t = txt;
        return ToString(t);
    }

    std::string Error::ToString(std::string& txt)
    {
        std::string result;

        switch(kind)
        {
            case Kind::Syntax:
            {
                result += "Syntax Error: ";
                break;
            }

            case Kind::Type:
            {
                result += "Type Error: ";
                break;
            }

            case Kind::Semantic:
            {
                result += "Semantic Error: ";
                break;
            }

            default:
            {
                std::string errdsc;
                errdsc = "Bad Error::Kind: " + dsc + ", " + txt;
                FatalError(errdsc.data(), __FILE__, __LINE__);
            }
        }

        result += dsc + "\n";
        result += txt + "\n";

        for (size_t i = 0; i < txt.size(); i++)
        {
            if (i < loc.firstColumn)
                result += "-";
            else if (i > loc.lastColumn)
                result += "-";
            else
                result += "^";
        }
        result += "\n";


        return result;
    }

    std::ostream& Error::Print(std::ostream& out, std::string& txt)
    {
        out << ToString(txt);
        return out;
    }

    std::ostream& Error::Print(std::ostream& out, const char* txt)
    {
        out << ToString(txt);
        return out;
    }

    void FatalError(const char* dsc, const char* file, size_t line)
    {
        std::cerr << "Fatal Error in file: " << file << ", line: " << line << ": " << dsc << std::endl;
        exit(1);
    }
}
