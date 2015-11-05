define("ace/mode/jascal_highlight_rules",["require","exports","module","ace/lib/oop","ace/mode/text_highlight_rules"], function(require, exports, module) {
    "use strict";

    var oop = require("../lib/oop");
    var TextHighlightRules = require("./text_highlight_rules").TextHighlightRules;
    var identifierRe = "[a-zA-Z_][a-zA-Z\\d_]*\\b";

    var JascalHighlightRules = function(options) {
        var keywordMapper = this.createKeywordMapper({
            "variable.language":
                "this|.class",
            "keyword":
                "begin|end|if|then|else|while|do|" +
                "private|public|protected|return|repeat|until|function|procedure|module|" +
                "class|interface|new|var|static|const|using",
            "support.type":
                "unsigned|byte|short|int|char|float|double|string",
            "constant.language":
                "nil",
            "constant.language.boolean": "true|false"
        }, "identifier");
        var kwBeforeRe = "case|do|else|finally|in|instanceof|return|throw|try|typeof|yield|void";

        var escapedRe = "\\\\(?:x[0-9a-fA-F]{2}|" + // hex
            "u[0-9a-fA-F]{4}|" + // unicode
            "[0-2][0-7]{0,2}|" + // oct
            "3[0-6][0-7]?|" + // oct
            "37[0-7]?|" + // oct
            "[4-7][0-7]?|" + //oct
            ".)";

        this.$rules = {
            "start" : [
                {
                    token : "comment",
                    regex : /\/\/.*/,
                }, {
                    token : "comment",
                    regex : /\/\*/,
                    next : "long_comment" 
                }, {
                    token : keywordMapper,
                    regex : identifierRe
                }, {
                    token : "keyword.operator",
                    regex : /--|\+\+|==|=|!=|!==|<=|>=|<<=|>>=|<>|<|>|!|&&|\|\||[!%&*+\-~\/^]=?/,
                    next  : "start"
                }, {
                    token : "constant.numeric",
                    regex : /[0-9]+(u|U)?/
                }, {
                    token : "string",
                    regex : /\"(\\\"|[^\"])*\"/
                }
            ],
            "long_comment" : [
                {
                    token : "comment",
                    regex : /.*\*\//,
                    next : "start"
                }, {
                    token : "comment",
                    regex : /.*/
                }
            ]
        };
        
        this.normalizeRules();
    };

    oop.inherits(JascalHighlightRules, TextHighlightRules);

    exports.JascalHighlightRules = JascalHighlightRules;
});

define("ace/mode/folding/jascal",["require","exports","module","ace/lib/oop","ace/range","ace/mode/folding/fold_mode"], function(require, exports, module) {
    "use strict";

    var oop = require("../../lib/oop");
    var Range = require("../../range").Range;
    var BaseFoldMode = require("./fold_mode").FoldMode;

    var JascalFoldMode = exports.FoldMode = function(commentRegex) {
        if (commentRegex) {
            this.foldingStartMarker = new RegExp(
                this.foldingStartMarker.source.replace(/\|[^|]*?$/, "|" + commentRegex.start)
            );
            this.foldingStopMarker = new RegExp(
                this.foldingStopMarker.source.replace(/\|[^|]*?$/, "|" + commentRegex.end)
            );
        }
    };
    oop.inherits(JascalFoldMode, BaseFoldMode);

    (function() {
        
        this.foldingStartMarker = /(\{|\[)[^\}\]]*$|^\s*(\/\*)/;
        this.foldingStopMarker = /^[^\[\{]*(\}|\])|^[\s\*]*(\*\/)/;
        this.singleLineBlockCommentRe= /^\s*(\/\*).*\*\/\s*$/;
        this.tripleStarBlockCommentRe = /^\s*(\/\*\*\*).*\*\/\s*$/;
        this.startRegionRe = /^\s*(\/\*|\/\/)#?region\b/;
        this._getFoldWidgetBase = this.getFoldWidget;
        this.getFoldWidget = function(session, foldStyle, row) {
            var line = session.getLine(row);
        
            if (this.singleLineBlockCommentRe.test(line)) {
                if (!this.startRegionRe.test(line) && !this.tripleStarBlockCommentRe.test(line))
                    return "";
            }
        
            var fw = this._getFoldWidgetBase(session, foldStyle, row);
        
            if (!fw && this.startRegionRe.test(line))
                return "start"; // lineCommentRegionStart
        
            return fw;
        };

        this.getFoldWidgetRange = function(session, foldStyle, row, forceMultiline) {
            var line = session.getLine(row);
            
            if (this.startRegionRe.test(line))
                return this.getCommentRegionBlock(session, line, row);
            
            var match = line.match(this.foldingStartMarker);
            if (match) {
                var i = match.index;

                if (match[1])
                    return this.openingBracketBlock(session, match[1], row, i);
                    
                var range = session.getCommentFoldRange(row, i + match[0].length, 1);
                
                if (range && !range.isMultiLine()) {
                    if (forceMultiline) {
                        range = this.getSectionRange(session, row);
                    } else if (foldStyle != "all")
                        range = null;
                }
                
                return range;
            }

            if (foldStyle === "markbegin")
                return;

            var match = line.match(this.foldingStopMarker);
            if (match) {
                var i = match.index + match[0].length;

                if (match[1])
                    return this.closingBracketBlock(session, match[1], row, i);

                return session.getCommentFoldRange(row, i, -1);
            }
        };
        
        this.getSectionRange = function(session, row) {
            var line = session.getLine(row);
            var startIndent = line.search(/\S/);
            var startRow = row;
            var startColumn = line.length;
            row = row + 1;
            var endRow = row;
            var maxRow = session.getLength();
            while (++row < maxRow) {
                line = session.getLine(row);
                var indent = line.search(/\S/);
                if (indent === -1)
                    continue;
                if  (startIndent > indent)
                    break;
                var subRange = this.getFoldWidgetRange(session, "all", row);
                
                if (subRange) {
                    if (subRange.start.row <= startRow) {
                        break;
                    } else if (subRange.isMultiLine()) {
                        row = subRange.end.row;
                    } else if (startIndent == indent) {
                        break;
                    }
                }
                endRow = row;
            }
            
            return new Range(startRow, startColumn, endRow, session.getLine(endRow).length);
        };
        this.getCommentRegionBlock = function(session, line, row) {
            var startColumn = line.search(/\s*$/);
            var maxRow = session.getLength();
            var startRow = row;
            
            var re = /^\s*(?:\/\*|\/\/|--)#?(end)?region\b/;
            var depth = 1;
            while (++row < maxRow) {
                line = session.getLine(row);
                var m = re.exec(line);
                if (!m) continue;
                if (m[1]) depth--;
                else depth++;

                if (!depth) break;
            }

            var endRow = row;
            if (endRow > startRow) {
                return new Range(startRow, startColumn, endRow, line.length);
            }
        };

    }).call(JascalFoldMode.prototype);
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

define("ace/mode/jascal", [ "require", "exports", "module", "ace/mode/matching_brace_outdent", "ace/mode/jascal_brace_outdent", "ace/mode/folding/jascal" ], function(require, exports, module) {
    "use strict";

    var oop = require("../lib/oop");
    // defines the parent mode
    var TextMode = require("./text").Mode;
    var Tokenizer = require("../tokenizer").Tokenizer;
    var MatchingBraceOutdent = require("./matching_brace_outdent").MatchingBraceOutdent;

    // defines the language specific highlighters and folding rules
    var JascalHighlightRules = require("./jascal_highlight_rules").JascalHighlightRules;
    var JascalFoldMode = require("./folding/jascal").JascalFoldMode;

    var Mode = function() {
        // set everything up
        this.HighlightRules = JascalHighlightRules;
        this.$outdent = new MatchingBraceOutdent();
        this.foldingRules = JascalFoldMode;
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
        
        this.$id = "ace/mode/jascal";
    }).call(Mode.prototype);

    exports.Mode = Mode;
});