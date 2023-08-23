void pushTimeToFirebase(struct tm *timeinfo,FirebaseData firebaseData, FirebaseJson jsonData) {
    // Format the time and date strings
    char timeStr[9]; // HH:MM:SS
    char dateStr[20]; // DD/MM/YYYY   TZ
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", timeinfo);
    strftime(dateStr, sizeof(dateStr), "%d/%m/%Y   %Z", timeinfo);

    // Add the formatted strings to JSON
    jsonData.add("time", timeStr);
    jsonData.add("date", dateStr);

    if (Firebase.pushJSON(firebaseData, "/door/alertTemp", jsonData)) {
        Serial.println("Time data sent to Firebase successfully");
    } else {
        Serial.println("Failed to send time data to Firebase");
        Serial.println(firebaseData.errorReason());
    }
}

void printLocalTime(FirebaseData firebaseData, FirebaseJson jsonData) {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }

    // Push time and date data to Firebase
    pushTimeToFirebase(&timeinfo,firebaseData,jsonData);
}