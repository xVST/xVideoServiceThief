<?php

class ExtractorParser
{
    private $workingDir;
    private $extractorPath;
    private $extractor;
    private $siteHtml;
    private $metadata;
    private $metadataChanged;
    private $constants;

    public function __construct($path, $workingDir = '')
    {
        $this->workingDir = $workingDir;
        $this->extractorPath = $path;
        $this->extractor = file_get_contents($path);
        // init metadata
        $this->metadata = ['sha1' => sha1($this->extractor), 'extractor' => basename($path)];
        // try to load previous metadata
        if (file_exists($workingDir.basename($path).'.json'))
        {
            if ($json = json_decode(file_get_contents($workingDir.basename($path).'.json'), true))
            {
                $sha1 = $this->metadata['sha1'];
                // determine if metadata changed
                $this->metadataChanged = $json['sha1'] !== $this->metadata['sha1'];
                // restore back the generated sha1
                $this->metadata = $json;
                $this->metadata['sha1'] = $sha1;
                $this->metadata['htmlError'] = $this->metadata['htmlError'] ?? false;
            }
        }
    }

    private function host()
    {
        $info = parse_url($this->metadata['url']);
        // return the url host
        return $info['host'] ?? null;
    }

    private function siteHtml()
    {
        $this->metadata['htmlError'] = false;
        // can download the site html?
        if ($this->metadata['url'] && $this->siteHtml === null)
        {
            $ctx = stream_context_create(['http' => ['timeout' => 10]]);
            // download html
            if (($this->siteHtml = @file_get_contents($this->metadata['url'], false, $ctx)) === false)
            {
                echo "\033[0;31m [Error downloading the site HTML ({$this->metadata['url']})]";
                // ok, annotate this
                $this->metadata['htmlError'] = true;
            }
        }
        return $this->siteHtml;
    }

    public function generateTemplate()
    {
        if ( ! isset($this->metadata['url']))
        {
            echo "\e[0;31m [Site URL not detected… Template not generated!]";
            return;
        }
        // load the template
        $template = file_get_contents($this->workingDir.'../_template.js');
        // replace year
        $template = @str_replace('%year%', date('Y'), $template);
        $template = @str_replace('%version%', $this->metadata['version'], $template);
        $template = @str_replace('%url%', $this->metadata['url'], $template);
        $template = @str_replace('%id%', str_replace('www.', '', $this->host()), $template);
        $template = @str_replace('%title%', $this->metadata['title'], $template);
        $template = @str_replace('%adultContent%', $this->metadata['+18'] > 0.5 ? 'true' : 'false', $template);
        $template = @str_replace('%musicSite%', $this->metadata['music'] > 0.5 ? 'true' : 'false', $template);
        // convert matchers to array
        if ($this->metadata['matchers'])
        {
            $count = count($this->metadata['matchers']);
            $matchers = "new Array(\n";
            foreach ($this->metadata['matchers'] as $index => $matcher)
            {
                $matchers .= '		'.json_encode($matcher);
                if ($index < $count - 1) $matchers .= ',';
                $matchers .= "\n";
            }
            $matchers .= "	)";
            $template = @str_replace('%matchers%', $matchers, $template);
        }
        else // no data
        {
            $template = @str_replace('%matchers%', null, $template);
        }
        // convert favicon data to array
        if ($this->metadata['faviconData'])
        {
            $values = str_split($this->metadata['faviconData'], 2);
            $count = count($values);
            $favicon = "new Array(\n		";
            foreach ($values as $index => $value)
            {
                $favicon .= '0x'.$value;
                // add comma
                if ($index < $count - 1) $favicon .= ',';
                // new line
                if (($index + 1)%15 === 0)
                {
                    $favicon .= "\n		";
                }
            }
            $favicon .= ")";
            $template = @str_replace('%favicon%', $favicon, $template);
        }
        else // no data
        {
            $template = @str_replace('%favicon%', 'null', $template);
        }
        // get the file name
        $fileName = basename($this->metadata['extractor'], '.py');
        // write to disk
        file_put_contents($this->workingDir.$fileName.'.js', $template);
    }

    public function parse($force = false)
    {
        if ( ! $force && $this->metadataChanged === false && ! $this->metadata['htmlError'])
        {
            echo "\033[0;34m [ok]";
            // continue
            return;
        }
        // echo updating message
        echo $this->metadataChanged === null ? "\033[0;32m [added]" : "\033[0;36m [modified]";
        // parse extractor
        $this->parseConstants();
        $this->metadata['url'] = $this->parseSiteUrl();
        $this->metadata['title'] = $this->parseSiteTitle();
        $this->metadata['matchers'] = $this->parseUrlRegexMatchers();
        $this->metadata['+18'] = $this->calculeAdultsSiteProbability();
        $this->metadata['music'] = $this->calculeMusicSiteProbability();
        $this->metadata['faviconUrl'] = $this->parseFavicon();
        $this->metadata['faviconData'] = $this->downloadFavicon();
        // increase version
        if ($this->metadataChanged === true || ! isset($this->metadata['version']))
        {
            $version = explode('.', $this->metadata['version'] ?? '1.0');
            $version[count($version) - 1]++;
            $this->metadata['version'] = implode('.', $version);
        }
        // write metadata to disk
        file_put_contents($this->workingDir.$this->metadata['extractor'].'.json', json_encode($this->metadata, JSON_PRETTY_PRINT));
    }

    private function parseConstants()
    {
        // get a list of all defined consts
        if (preg_match_all("/(_[A-Z_]+)\s*=\s*(.*)/", $this->extractor, $matches))
        {
            $this->constants = array_combine($matches[1], $matches[2]);
        }
    }

    private function parseUrlRegexMatchers()
    {
        $result = [];
        // regex which start using " instead of '
        if (preg_match_all("/\s_VALID_URL\s*=\s*r(?:'|\")+([\s\S]+?)(?:'|\")+(.*)/", $this->extractor, $matches))
        {
            foreach ($matches[2] as $key => &$value)
            {
                // remove the plus operator
                $value = str_replace('+', '', $value);
                // replace constants
                foreach ($this->constants as $name => $cValue)
                {
                    if (preg_match("/(?:'|\")+([\s\S]+?)(?:'|\")+/", $cValue, $cMatches))
                    {
                        $cValue = $cMatches[1];
                    }
                    // replace const
                    $value = str_replace($name, $cValue, $value);
                }
                // mix both values (regex and consts) and remove spaces
                $result[] = str_replace(' ', '', $matches[1][$key].$matches[2][$key]);
            }
        }
        //var_dump($result);
        return $result;
    }

    private function parseSiteUrl()
    {
        if (preg_match("/_TESTS?\s*=\s*\[?\s*{([\s\S]+?)}/", $this->extractor, $matches))
        {
            if (preg_match("/'url'\s*:\s*'(.*?)'/", $matches[1], $info))
            {
                $url = parse_url($info[1]);
                // build the "main" url
                return isset($url['host']) ? $url['scheme'].'://'.$url['host'].'/' : null;
            }
        }
        return null;
    }

    private function calculeProbability(array $words)
    {
        // some inits
        $rank = $total = 0;
        // convert to lower case
        if ($html = $this->metadata['url']."\n".mb_strtolower($this->siteHtml()))
        {
            // look for each word
            foreach ($words as $points => $list)
            {
                foreach ($list as $word)
                {
                    if (preg_match("/(\b$word\b)/im", $html, $matches))
                    {
                        $rank += $points;
                    }
                    // calcule total points
                    $total += $points;
                }
            }
            // % of probability
            return $rank/$total;
        }
        return null;
    }

    private function calculeAdultsSiteProbability()
    {
        static $words =
            [
                15 => ['sex', 'porn', 'porno', 'milf', 'hentai', 'blowjob', 'bukkake', 'orgy'],
                12 => ['anal', 'dick', 'pussy', 'fucked'],
                8  => ['nsfw', 'violence', 'forced'],
                5  => ['gay', 'shemale', 'facial', 'ass'],
                4  => ['sexy', 'mature', 'spy', 'disgusting'],
                1  => ['hetero', 'bathroom', 'shower'],
            ];
        // calcule probability
        return $this->calculeProbability($words);
    }

    private function calculeMusicSiteProbability()
    {
        static $words =
            [
                10 => ['rock', 'hip-hop', 'jazz', 'folk', 'disco', 'heavy metal', 'podcast', 'podcast', 'listener'],
                8  => ['radio', 'music', 'videoclip', 'audio', 'mp3'],
                5  => ['artist', 'tour', 'live'],
            ];
        // calcule probability
        return $this->calculeProbability($words);
    }

    private function parseSiteTitle()
    {
        if ($html = $this->siteHtml())
        {
            $doc = new DOMDocument();
            @$doc->loadHTML($html);
            // get the title tag
            if ($elements = $doc->getElementsByTagName('title'))
            {
                if ($elements->length)
                {
                    return trim($elements->item(0)->textContent);
                }
            }
        }
        // no title found, so we return the domain as a title
        return $this->host();
    }

    private function parseFavicon()
    {
        if ($html = $this->siteHtml())
        {
            $doc = new DOMDocument();
            @$doc->loadHTML($html);
            // get the link elements
            if ($elements = $doc->getElementsByTagName('link'))
            {
                $favicon = null;
                $lastPoints = 0;
                // get all the potential favicons
                foreach ($elements as $element)
                {
                    $points = 0;
                    // is an icon?
                    if (strpos(strtolower($element->getAttribute('rel')), 'shortcut icon') !== false)
                    {
                        $points = 10;
                    }
                    elseif (strtolower($element->getAttribute('rel')) === 'icon')
                    {
                        $points = 9;
                    }
                    elseif (strpos(strtolower($element->getAttribute('rel')), 'apple-touch-icon') !== false)
                    {
                        if ($element->getAttribute('sizes') === '57x57')
                        {
                            $points = 8;
                        }
                        elseif ($element->getAttribute('sizes') === '72x72')
                        {
                            $points = 7;
                        }
                    }
                    // get the best one
                    if ($lastPoints < $points)
                    {
                        $lastPoints = $points;
                        $favicon = $element->getAttribute('href');
                    }
                }
                // is not a complete url?
                if ( ! empty($favicon) && strpos($favicon, "http") !== 0)
                {
                    $favicon = $this->metadata['url'].$favicon;
                }
                return $favicon;
            }
        }
        return null;
    }

    private function downloadFavicon()
    {
        if ($this->metadata['faviconUrl'] && $data = @file_get_contents($this->metadata['faviconUrl']))
        {
            return bin2hex($data);
        }
        return null;
    }
}

$py = "youtube-dl/youtube_dl/extractor/*.py";
$dir = "generated/";

$ignore = ['__init__.py', 'testurl.py', 'common.py', 'commonprotocols,py', 'commonmistakes.py'];

foreach (glob($py) as $file)
{
    // parse all extractors
    if ( ! in_array(basename($file), $ignore))
    {
        echo "\e[0;33m ".basename($file);
        $parser = new ExtractorParser($file, $dir);
        $parser->parse();
        $parser->generateTemplate();
        echo PHP_EOL;
    }
}

