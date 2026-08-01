/**
 * The "Simplified BSD License"
 *
 * Copyright (c) 2025, Yurii Sydor (yuriysydor1991@gmail.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_CURLCONTROLLER_CLASS_H
#define IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_CURLCONTROLLER_CLASS_H

#include <curl/curl.h>

#include <memory>
#include <string>
#include <vector>

/**
 * @brief The libcurl adaptor subsystem namespace.
 */
namespace iannotator::exporters::curli
{

/**
 * @brief The libcurl downloader class.
 */
class CURLController
{
 public:
  using download_buffer = std::vector<char>;
  using CURLControllerPtr = std::shared_ptr<CURLController>;

  inline static unsigned long long DEFAULT_TIMEOUT = 30L;
  inline static unsigned long long DEFAULT_CONNECTTIMEOUT = 10L;
  inline static unsigned long long DEFAULT_LOWSPEEDSECS = 10L;
  inline static unsigned long long DEFAULT_LOWSPEEDLIMIT = 1L;

  virtual ~CURLController();
  CURLController();
  CURLController(const CURLController&) = delete;
  CURLController(CURLController&&) = delete;

  virtual download_buffer& download(const std::string& url);

  virtual download_buffer& get();

  static CURLControllerPtr create();

  static std::string get_absolute_url(const std::string& hostPath,
                                      const std::string& relPath);

  static bool is_url(const std::string& maybe);

  static std::string get_url_hostname(const std::string& url);

  virtual std::string get_last_download_mime();

  virtual std::string get_url_path(const std::string& url);

 private:
  static constexpr const download_buffer::size_type DEFAULT_BUFF_RESERVE =
      10240U;

  static std::string get_absolute_url(CURLU* resolved_url,
                                      const std::string& hostPath,
                                      const std::string& relPath);

  download_buffer cbuff;
  CURL* curl{nullptr};
};

using CURLControllerPtr = CURLController::CURLControllerPtr;

}  // namespace iannotator::exporters::curli

#endif  // IMAGES_ANNOTATOR_DATA_EXPORTERS_PROJECT_CURLCONTROLLER_CLASS_H
