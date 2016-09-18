struct RgbUrl {
    bool red;
    bool green;
    bool blue;

    RgbUrl() : red(false), green(false), blue(false) {}

    void from_query(const char * query, unsigned int n) {
        const char * const end = query + n;
        while (query != end && *query != '\0' && *query != ' ') {
            bool * current = nullptr;
            if (*(query + 1) == '=') {
                switch (*query) {
                    case 'r':
                        current = &red;
                        break;
                    case 'g':
                        current = &green;
                        break;
                    case 'b':
                        current = &blue;
                        break;
                }
            }
            while (query != end && *query != '\0' && *query != ' ' && *query != '=' && *query != '&') {
                ++query;
            }
            if (*query == '=') {
                ++query;
            }
            if (current != nullptr) {
                *current = *query != '0';
            }
            while (query != end && *query != '\0' && *query != ' ' && *query != '&') {
                ++query;
            }
            if (*query == '&') {
                ++query;
            }
        }
    }

    void dump(char * buf) {
        *buf++ = 'r';
        *buf++ = '=';
        *buf++ = red ? '1' : '0';
        *buf++ = '&';
        *buf++ = 'g';
        *buf++ = '=';
        *buf++ = green ? '1' : '0';
        *buf++ = '&';
        *buf++ = 'b';
        *buf++ = '=';
        *buf++ = blue ? '1' : '0';
        *buf = '\0';
    }
};
