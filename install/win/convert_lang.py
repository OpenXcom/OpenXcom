import glob

# Convert YAML language files to NSIS format
LANGS = {
	"bg": "Bulgarian",
	"ca-ES": "Catalan",
	"cs": "Czech",
	"cy": "Welsh",
	"da": "Danish",
	"de": "German",
	"el": "Greek",
	"et": "Estonian",
	"es-ES": "Spanish",
	"es-419": "SpanishInternational",
	"fr": "French",
	"fi": "Finnish",
	"ga": "Galician",
	"hr": "Croatian",
	"hu": "Hungarian",
	"it": "Italian",
	"is": "Icelandic",
	"ja": "Japanese",
	"ko": "Korean",
	"lb": "Luxembourgish",
	"lv": "Latvian",
	"nl": "Dutch",
	"no": "Norweigan",
	"pl": "Polish",
	"pt-BR": "PortugueseBR",
	"pt-PT": "Portuguese",
	"ro": "Romanian",
	"ru": "Russian",
	"sk": "Slovak",
	"sl": "Slovenian",
	"sv": "Swedish",
	"tr": "Turkish",
	"uk": "Ukrainian",
	"vi": "Vietnamese",
	"zh-CN": "SimpChinese",
	"zh-TW": "TradChinese",
}

langs = []
for yamlPath in glob.glob("Language/*.yml"):
    nshLines = []
    nshLang = ""
    with open(yamlPath, "r", encoding="utf-8") as yamlFile:
        for line in yamlFile:
            # First line is the language ID
            if '"' not in line:
                lang = line.strip(":\n")
                if lang in LANGS:
                    nshLang = LANGS[lang]
                    nshLines.append(f'!insertmacro LANGFILE_EXT "{nshLang}"\n')
                    langs.append(nshLang)
                else:
                    break
            else:
                string = line.strip().replace(': "', ' "').replace("\\\"", "")
                nshLine = "${LangFileString} " + string + "\n"
                nshLines.append(nshLine)

    if nshLang != "":
        nshPath = "Language/" + nshLang + ".nsh"
        with open(nshPath, "w", encoding="utf-8") as nshFile:
            nshFile.writelines(nshLines)

with open("language.nsh", "w", encoding="utf-8") as nshInclude:
    nshInclude.write('!insertmacro MUI_LANGUAGE "English"\n')
    nshInclude.write('!insertmacro LANGFILE_INCLUDE "Language\\English.nsh"\n')
    for lang in langs:
        nshInclude.write(f'!insertmacro MUI_LANGUAGE "{lang}"\n')
        nshInclude.write(f'!insertmacro LANGFILE_INCLUDE "Language\\{lang}.nsh"\n')