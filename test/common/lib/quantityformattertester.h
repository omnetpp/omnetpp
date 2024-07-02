#ifndef __OMNETPP_TEST_QUANTITYFORMATTERTESTER_H
#define __OMNETPP_TEST_QUANTITYFORMATTERTESTER_H

#include <common/quantityformatter.h>

using namespace omnetpp::common;

static std::string digits123etc(int numDigits) {
    std::stringstream stream;
    for (int i = 0; i < numDigits; i++)
        stream << (i + 1);
    return stream.str();
}

static std::string digits987etc(int numDigits) {
    std::stringstream stream;
    for (int i = 0; i < numDigits; i++)
        stream << (9 - i);
    return stream.str();
}

static std::string digits9etc(int numDigits) {
    std::stringstream stream;
    for (int i = 0; i < numDigits; i++)
        stream << 9;
    return stream.str();
}

static std::string digits10etc1(int numDigits) {
    std::stringstream stream;
    stream << 1;
    for (int i = 0; i < numDigits; i++)
        stream << 0;
    stream << 1;
    return stream.str();
}

static std::string digits4etc5(int numDigits) {
    std::stringstream stream;
    for (int i = 0; i < numDigits; i++)
        stream << (i == numDigits - 1 ? 5 : 4);
    return stream.str();
}

static void testQuantityFormatter(QuantityFormatter& formatter, std::string text, const char *unit) {
    double value = strtod(text.c_str(), nullptr);
    auto output = formatter.formatQuantity(value, unit);
    auto simplified = output.text;
    simplified = opp_replacesubstring(simplified, "~", "", true);
    simplified = opp_replacesubstring(simplified, ",", "", true);
    double parsed = std::numeric_limits<double>::quiet_NaN();
    try {
        parsed = UnitConversion::parseQuantity(simplified.c_str(), unit);
    }
    catch (...) {
    }
    double absoluteError = value - parsed;
    double relativeError = value == 0 && absoluteError == 0 ? 0 : std::abs(absoluteError / value);
    std::stringstream valuestream;
    valuestream << std::fixed << std::setprecision(24) << value;
    if (unit) {
        text += " " + std::string(unit);
        valuestream << " " << unit;
    }
    double maxRelativeError = std::pow(10, 1 - formatter.getOptions().maxSignificantDigits) / 2;
    int numDigits = 0;
    int numLeadingZeros = 0;
    int numTrailingZeros = 0;
    bool leadingZeros = true;
    bool integer = true;
    for (int i = 0; i < output.role.size(); i++) {
        if (output.role[i] == 'd') {
            if (output.text[i] == '0') {
                if (leadingZeros)
                    numLeadingZeros++;
                else
                    numTrailingZeros++;
            }
            else {
                leadingZeros = false;
                numTrailingZeros = 0;
            }
            numDigits++;
        }
        else if (output.role[i] == '.')
            integer = false;
        else if (output.role[i] == 'e')
            break;
    }
    int numSignificantDigits = numDigits - numLeadingZeros - numTrailingZeros;
    const char *errorMessage = nullptr;
    if (relativeError > maxRelativeError)
        errorMessage = "Relative error too large";
    else if (value != 0 && numSignificantDigits < formatter.getOptions().minSignificantDigits)
        errorMessage = "Too few significant digits";
    else if (!integer && numSignificantDigits > formatter.getOptions().maxSignificantDigits)
        errorMessage = "Too many significant digits";
    std::cout << std::setw(24) << std::left << text \
              << std::setw(40) << std::left << valuestream.str()
              << std::setw(18) << std::left << output.text
              << std::setw(18) << std::left << parsed
              << std::setw(18) << std::left << (std::isnan(absoluteError) ? NAN : absoluteError)
              << std::setw(18) << std::left << (std::isnan(relativeError) ? NAN : relativeError)
              << (errorMessage ? "!!! " : "") << (errorMessage ? errorMessage : "")
              << std::endl;
}

static std::vector<std::string> generateNumbers(int maxDigits, int minScale, int maxScale, std::vector<std::function<std::string(int)>> computeDigits) {
    std::vector<std::string> result;
    for (auto sign : {"", "-"}) {
        for (int numDigits = 1; numDigits <= maxDigits; numDigits++) {
            for (int scale = minScale; scale <= maxScale; scale++) {
                for (auto f : computeDigits) {
                    auto digits = f(numDigits);
                    int shift = scale - numDigits + 1;
                    if (shift > 0)
                        for (int i = 0; i < shift; i++)
                            digits = digits + "0";
                    else if (shift < 0) {
                        int c = -shift - digits.size() + 1;
                        for (int i = 0; i < c; i++)
                            digits = "0" + digits;
                        digits = digits.substr(0, digits.size() + shift) + "." + digits.substr(digits.size() + shift);
                    }
                    digits = sign + digits;
                    result.push_back(digits);
                }
            }
        }
    }
    return result;
}

static void testQuantityFormatter(QuantityFormatter& formatter, const std::vector<std::string>& numbers, const char *unit) {
    std::cout << "Text input, printf-ed double, formatted quantity, parsed formatted quantity, absolute error, relative error" << std::endl;
    for (std::string number : numbers)
        testQuantityFormatter(formatter, number.c_str(), unit);
}

static void testQuantityFormatter(QuantityFormatter& formatter, int maxDigits, int minScale, int maxScale, const char *unit, std::vector<std::function<std::string(int)>> computeDigits) {
    testQuantityFormatter(formatter, generateNumbers(maxDigits, minScale, maxScale, computeDigits), unit);
}

static void testQuantityFormatter(QuantityFormatter& formatter, int size, const char *unit, std::vector<std::function<std::string(int)>> computeDigits) {
    testQuantityFormatter(formatter, generateNumbers(size, -size, size, computeDigits), unit);
}

#endif

