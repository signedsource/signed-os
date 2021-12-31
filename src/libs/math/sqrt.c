long float sqrt(long float number) {
    long float result = number;
    long float lastResult = 0;
    while (result != lastResult) {
        lastResult = result;
        result = (result + number / result) / 2;
    }
    
    return result;
}
