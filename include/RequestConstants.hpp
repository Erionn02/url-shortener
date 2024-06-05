#include <regex>

namespace requests {
    namespace headers {
        static inline auto URL_TO_SHORTEN{"url_to_shorten"};
        static inline auto CUSTOM_PATH{"custom_path"};
    }
    namespace errors {
        static inline auto URL_TO_SHORTEN_MISSING{fmt::format("Header {} is missing!", headers::URL_TO_SHORTEN)};
        static inline auto GIVEN_URL_IS_INVALID_OR_NOT_RESPONSIVE{"Given url is invalid or not responsive."};
        static inline auto URL_TO_SHORTEN_CANNOT_LEAD_TO_THIS_SERVER{"Url to shorten cannot lead to this server."};
        static inline auto GIVEN_CUSTOM_URL_IS_FORBIDDEN{"Given custom url is forbidden."};
        static inline auto GIVEN_URL_ALREADY_EXISTS{"Given url already exists."};
        static inline auto GIVEN_CUSTOM_PATH_IS_TOO_LONG{"Given url custom path is too long."};
    }

    static inline std::regex URL_VALIDATOR{"((http|https)://)?(www\\.)?([a-zA-Z0-9.-]+)(\\.[a-zA-Z]{2,})(:\\d+)?(/[a-zA-Z0-9&%_./-~-]*)?"};
}