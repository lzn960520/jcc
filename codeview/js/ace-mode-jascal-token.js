define("ace/mode/jascal_token_highlight_rules",["require","exports","module","ace/lib/oop","ace/mode/text_highlight_rules"], function(require, exports, module) {
    "use strict";

    var oop = require("../lib/oop");
    var TextHighlightRules = require("./text_highlight_rules").TextHighlightRules;
    var identifierRe = "[a-zA-Z_][a-zA-Z\\d_]*\\b";

    var JascalTokenHighlightRules = function(options) {
        this.$rules = {
            "start" : [
                {
                    token : "invisible",
                    regex : /</,
                    next : "in_token"
                }
            ],
            "in_token" : [
                {
                    token : "comment",
                    regex : /comment/,
                    next : "expect_out_token"
                }, {
                    token : "identifier",
                    regex : /identifier [^>]*/,
                    next : "expect_out_token"
                }, {
                    token : "keyword",
                    regex : /\.\.|::/,
                    next : "expect_out_token"
                }, {
					token : "keyword",
					regex : /:|;/,
					next : "expect_out_token"
				}, {
					token : "keyword.operator",
					regex : /<<=|>>=|\*\*=|\|\|=|&&=|^^=/,
					next : "expect_out_token"
				}, {
					token : "keyword.operator",
					regex : /\+\+|--|&&|\^\^|\|\||<=|>=|\*\*|==|!=|&=|^=|\|=|\+=|-=|\*=|\/=|%=|<<|>>(?=>)|<>(?=>)/,
					next : "expect_out_token"
				}, {
                    token : "keyword.operator",
                    regex : /[+-\/*\|&^%<>()\[\]=~!]/,
                    next : "expect_out_token"
                }, {
                    token : "constant.numeric",
                    regex : /literal_int [^>]*|literal_float [^>]*|literal_double [^>]*/,
                    next : "expect_out_token"
                }, {
                    token : "string",
                    regex : /literal_string \"(\\\"|[^\"])*\"/,
                    next : "expect_out_token"
                }, {
					token : "keyword",
					regex : /interface/,
					next : "expect_out_token"
				}, {
                    token : "support.type",
                    regex : /int|byte|short|char|unsigned|string|float|double/,
                    next : "expect_out_token"
                }, {
                    token : "keyword",
                    regex : /[#a-z_]*/,
                    next : "expect_out_token"
                }
            ],
            "expect_out_token" : [
                {
                    token : "invisible",
                    regex : />/,
                    next : "start"
                }
            ]
        };

        this.normalizeRules();
    };

    oop.inherits(JascalTokenHighlightRules, TextHighlightRules);

    exports.JascalTokenHighlightRules = JascalTokenHighlightRules;
});

define("ace/mode/folding/jascal_token",["require","exports","module","ace/lib/oop","ace/range","ace/mode/folding/fold_mode"], function(require, exports, module) {
    "use strict";

    var oop = require("../../lib/oop");
    var Range = require("../../range").Range;
    var BaseFoldMode = require("./fold_mode").FoldMode;

    var JascalTokenFoldMode = exports.JascalTokenFoldMode = function(commentRegex) {
        if (commentRegex) {
            this.foldingStartMarker = new RegExp(
                this.foldingStartMarker.source.replace(/\|[^|]*?$/, "|" + commentRegex.start)
            );
            this.foldingStopMarker = new RegExp(
                this.foldingStopMarker.source.replace(/\|[^|]*?$/, "|" + commentRegex.end)
            );
        }
    };
    oop.inherits(JascalTokenFoldMode, BaseFoldMode);

    (function() {

        this.foldingStartMarker = /<BEGIN>/;
        this.foldingStopMarker = /<END>/;
        this.singleLineBlockCommentRe= /^\s*(\/\*).*\*\/\s*$/;
        this.tripleStarBlockCommentRe = /^\s*(\/\*\*\*).*\*\/\s*$/;
        this._getFoldWidgetBase = this.getFoldWidget;
        this.getFoldWidget = function(session, foldStyle, row) {
            var line = session.getLine(row);

            if (this.singleLineBlockCommentRe.test(line)) {
                if (!this.startRegionRe.test(line) && !this.tripleStarBlockCommentRe.test(line))
                    return "";
            }

            var fw = this._getFoldWidgetBase(session, foldStyle, row);

            return fw;
        };

        this.getFoldWidgetRange = function(session, foldStyle, row, forceMultiline) {
            var line = session.getLine(row);

            var match = line.match(this.foldingStartMarker);
            if (match) {
                var i = match.index;

                if (match[1])
                    return this.openingBracketBlock(session, match[1], row, i);
            }

            if (foldStyle === "markbegin")
                return;

            var match = line.match(this.foldingStopMarker);
            if (match) {
                var i = match.index + match[0].length;

                if (match[1])
                    return this.closingBracketBlock(session, match[1], row, i);
            }

            return null;
        };

    }).call(JascalTokenFoldMode.prototype);
});

define("ace/mode/matching_brace_outdent",["require","exports","module","ace/range"], function(require, exports, module) {
    "use strict";

    var Range = require("../range").Range;

    var MatchingBraceOutdent = function() {};

    (function() {

        this.checkOutdent = function(line, input) {
            if (! /^\s+$/.test(line))
                return false;

            return /^\s*\}/.test(input);
        };

        this.autoOutdent = function(doc, row) {
            var line = doc.getLine(row);
            var match = line.match(/^(\s*\})/);

            if (!match) return 0;

            var column = match[1].length;
            var openBracePos = doc.findMatchingBracket({row: row, column: column});

            if (!openBracePos || openBracePos.row == row) return 0;

            var indent = this.$getIndent(doc.getLine(openBracePos.row));
            doc.replace(new Range(row, 0, row, column-1), indent);
        };

        this.$getIndent = function(line) {
            return line.match(/^\s*/)[0];
        };

    }).call(MatchingBraceOutdent.prototype);

    exports.MatchingBraceOutdent = MatchingBraceOutdent;
});

define("ace/mode/jascal_token", [ "require", "exports", "module", "ace/mode/matching_brace_outdent", "ace/mode/jascal_token_highlight_rules", "ace/mode/folding/jascal_token" ], function(require, exports, module) {
    "use strict";

    var oop = require("../lib/oop");
    // defines the parent mode
    var TextMode = require("./text").Mode;
    var Tokenizer = require("../tokenizer").Tokenizer;
    var MatchingBraceOutdent = require("./matching_brace_outdent").MatchingBraceOutdent;

    // defines the language specific highlighters and folding rules
    var JascalTokenHighlightRules = require("./jascal_token_highlight_rules").JascalTokenHighlightRules;
    var JascalTokenFoldMode = require("./folding/jascal_token").JascalTokenFoldMode;

    var Mode = function() {
        // set everything up
        this.HighlightRules = JascalTokenHighlightRules;
        this.$outdent = new MatchingBraceOutdent();
        this.foldingRules = new JascalTokenFoldMode();
    };
    oop.inherits(Mode, TextMode);

    (function() {
        // configure comment start/end characters
        this.lineCommentStart = "//";
        this.blockComment = {start: "/*", end: "*/"};

        // special logic for indent/outdent.
        // By default ace keeps indentation of previous line
        this.getNextLineIndent = function(state, line, tab) {
            var indent = this.$getIndent(line);
            return indent;
        };

        this.checkOutdent = function(state, line, input) {
            return this.$outdent.checkOutdent(line, input);
        };

        this.autoOutdent = function(state, doc, row) {
            this.$outdent.autoOutdent(doc, row);
        };

        // create worker for live syntax checking
        this.createWorker = function(session) {
            return null;
        };

        this.$id = "ace/mode/jascal_token";
    }).call(Mode.prototype);

    exports.Mode = Mode;
});
