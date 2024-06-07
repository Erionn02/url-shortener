#include <regex>

namespace requests {
    namespace headers {
        static inline auto URL_TO_SHORTEN{"url_to_shorten"};
        static inline auto CUSTOM_PATH{"custom_path"};
    }
    namespace errors {
        static inline auto HEADER_URL_TO_SHORTEN_MISSING{fmt::format("Header {} is missing!", headers::URL_TO_SHORTEN)};
        static inline auto GIVEN_URL_IS_NOT_RESPONSIVE{"Given url is not responsive."};
        static inline auto GIVEN_URL_IS_INVALID{"Given url is invalid."};
        static inline auto URL_TO_SHORTEN_CANNOT_LEAD_TO_THIS_SERVER{"Url to shorten cannot lead to this server."};
        static inline auto GIVEN_CUSTOM_PATH_IS_FORBIDDEN{"Given custom path is forbidden."};
        static inline auto GIVEN_PATH_ALREADY_EXISTS{"Given path already exists."};
        static inline auto GIVEN_CUSTOM_PATH_IS_TOO_LONG{"Given custom path is too long."};
        static inline auto GIVEN_CUSTOM_PATH_IS_TOO_SHORT{"Given custom path is too short."};
        static inline auto GIVEN_CUSTOM_PATH_STARTS_WITH_FORBIDDEN_CHARACTER{"Given custom path starts with forbidden character."};
    }
}