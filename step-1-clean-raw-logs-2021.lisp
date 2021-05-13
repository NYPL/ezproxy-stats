#!/usr/local/bin/lispscript


(defvar /all-logs/ (zsh "find logs | ack '2021-' | ack 'log$' | sort" :split t))

; remove last (incomplete) log entry
(setq /all-logs/ (reverse (cdr (reverse /all-logs/))))
(defvar /count/ (length /all-logs/))

(defconstant +updated-date+ (~e (car (last /all-logs/)) •(\d{4}-\d{2}-\d{2})•))
(defconstant +output-file+ (fn "intermediate/cleaned-logs-~A.dat" +updated-date+))

(format *error-output* (yellow "about to process files~%~%"))

(with-a-file +output-file+  :w
  (format stream! "~A~%" (delim '("ip" "barcode" "session"
                                  "date_and_time" "url" "fullurl")))
  (for-each/list /all-logs/
    (progress-bar index! /count/ :out-of t)
    (for-each/line value!
      « (destructuring-bind
          (ip barcode sessionp date garb1 garb2 url garb3 garb4 garb5)
          (~s value! " ")

          (setq fullurl url)

          ; exclusions
          (when (string= "-" barcode)
            (continue!))

          ; cleaning
          (setq url   (~r url "^https?[^A-Za-z]*" ""))
          (setq url   (~r url ":.+" ""))
          (setq date  (~r date •\[(\d+)/(.+?)/(\d+):(\d+):(\d+):(\d+)•
                               •\3-\2-\1 \4:\5:\6•))

          ; other months
          (setq date (~r date "Jan" "01"))
          (setq date (~r date "Feb" "02"))
          (setq date (~r date "Mar" "03"))
          (setq date (~r date "Apr" "04"))
          (setq date (~r date "May" "05"))
          (setq date (~r date "Jun" "06"))
          (setq date (~r date "Jul" "07"))
          (setq date (~r date "Aug" "08"))
          (setq date (~r date "Sep" "09"))
          (setq date (~r date "Oct" "10"))
          (setq date (~r date "Nov" "11"))
          (setq date (~r date "Dec" "12"))

          (format stream! "~A~%" (delim (list ip barcode sessionp
                                              date url fullurl))))
        or do (continue!) » )))


