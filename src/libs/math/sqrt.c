float sqrt(float number) {
    float result = number;
    float lastResult = 0;
    while (result != lastResult) {
        lastResult = result;
        result = (result + number / result) / 2;
    }
    
    return result;
}
