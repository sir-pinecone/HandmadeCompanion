#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from __future__ import print_function
import codecs
import sys
import requests
from HTMLParser import HTMLParser
import time
import datetime
import StringIO

class MyHTMLParser(HTMLParser):
    def __init__(self):
        self.inside_entry_tag = False
        self.entry = []
        self.entries = []
        HTMLParser.__init__(self)

    def handle_starttag(self, tag, attrs):
        if tag == 'entry':
            self.inside_entry_tag = True
        elif tag in ('title', 'id', 'published'):
            self.record_data = True
        else:
            self.record_data = False

    def handle_endtag(self, tag):
        if tag == 'entry':
            self.inside_entry_tag = False
            self.entries.append(self.entry)
            self.entry = []

    def handle_data(self, data):
        if self.inside_entry_tag and self.record_data:
            text = data.strip()
            if text:
                self.entry.append(text)

def canonical(s):
    words = []
    word = []
    for c in s.lower():
        if c.isalpha():
            word.append(c)
        elif word:
            words.append(''.join(word))
            word = []
    # last word
    if word:
        words.append(''.join(word))

    return words

def best_match(text, sentence):
    text_len = len(text)
    sentence_len = len(sentence)
    best_match_score = -1.
    best_match_index = None
    for start_word_index in range(0, text_len-sentence_len):
        subtext = text[start_word_index:start_word_index+sentence_len]
        score = 0.
        for a, b in zip(subtext, sentence):
            if a == b:
                score += 1
        score /= sentence_len
        if score > best_match_score:
            best_match_score = score
            best_match_index = start_word_index

    return best_match_score, best_match_index

# Choose episode
if len(sys.argv) != 2:
    print('Usage:', sys.argv[0], 'episode_number')
    sys.exit(1)             ######################################## EXIT

episode_number = sys.argv[1]
print('Looking for episode {}'.format(episode_number))

# Get list of last videos
r = requests.get('https://www.youtube.com/feeds/videos.xml?user=handmadeheroarchive')

# Extract title, URL and date of videos
# Look for target episode
parser = MyHTMLParser()
parser.feed(r.text)

youtube_video_id = title = data = None
found = False
for youtube_video_id, title, date in parser.entries:
    if episode_number in title.split():
        found = True
        break

if not found:
    print('Could not find target episode')
    sys.exit(1)             ######################################## EXIT


yttitle = title
title = title[title.find('-')+1 :].strip()
youtube_video_id = youtube_video_id[youtube_video_id.rfind(':')+1:]

print('\n================================================================================\n')
print('New Video: %s\n' %(format(yttitle)))
print('Needs annotating: https://www.youtube.com/watch?v=%s' %(format(youtube_video_id)))
if "Handmade Hero Chat" in yttitle:
    print('https://hero.handmade.network/episode/chat/chat%03d\n' %(int(episode_number)))
else:
    print('https://hero.handmade.network/episode/game-architecture/day%3d\n' %(int(episode_number)))
print('Ensure the ToC has been updated!\n')
print('================================================================================\n')

if "Handmade Hero Chat" in yttitle:
    genre = "chat"
else:
    genre = "day"

# Write basic template for video
with open('%s%03d.html.md'%(genre, int(episode_number)), 'w') as f:
    f.write('---\n')
    f.write('title: "{}"\n'.format(title))
    f.write('videoId: "{}"\n'.format(youtube_video_id))
    f.write('markers:\n')
    f.write('---\n')

# See if there are automatic captions for this episode
keepsubs_request = 'http://keepsubs.com/?url=https://www.youtube.com/watch?v={}'
r = requests.get(keepsubs_request.format(youtube_video_id))

youtube_captions = None
for line in r.text.split('\n'):
    if 'automatic captions' in line.lower():
        has_captions = True
        start_index = line.find('a href="http://keepsubs.com/subs/youtube.com.php') + 8
        length = line[start_index:].find('"')
        url = line[start_index:start_index+length]
        r = requests.get(url)
        youtube_captions = r.text
        break

if not youtube_captions:
    print('No automatic youtube captions found')
else:
    print('Found automatic youtube captions')

# Get twitch chat log
headers = {'Accept': 'application/vnd.twitchtv.v3+json'}
url = 'https://api.twitch.tv/kraken/channels/handmade_hero/videos?limit=100&broadcasts=true'

r = requests.get(url, headers=headers)
r_json = r.json()
n_videos = len(r_json['videos'])

youtube_timestamp = time.mktime(datetime.datetime.strptime(date[:date.find('+')], '%Y-%m-%dT%H:%M:%S').timetuple())

target_twitch_video_entries = []
for video_desc in r_json['videos']:
    twitch_time = video_desc['recorded_at']
    twitch_timestamp = time.mktime(datetime.datetime.strptime(twitch_time, '%Y-%m-%dT%H:%M:%SZ').timetuple())

    # twitch video time prior to youtube post, but not more than 24h
    if (youtube_timestamp > twitch_timestamp and
        youtube_timestamp-twitch_timestamp < 24*60*60):
        target_twitch_video_entries.append((video_desc['_id'], twitch_time))

target_twitch_video_entries = sorted(target_twitch_video_entries)

if not target_twitch_video_entries:
    print('Could not locate matching twitch chat logs')
    sys.exit(1)             ######################################## EXIT

print('Found matching twitch chat logs:')
for twitch_video_id, twitch_time in target_twitch_video_entries:
    print('\t{} - {}'.format(twitch_video_id, twitch_time))

# Parse twitch questions
process = False
twitch_qs = []
for twitch_video_id, twitch_video_time in target_twitch_video_entries:
    url = 'http://search.rechat.org/videos/{}?include_jtv=true&after={}'
    r = requests.get(url.format(twitch_video_id, twitch_video_time))

    while len(r.json()['hits']['hits']):
        for hit in r.json()['hits']['hits']:
            if hit['_type'] == 'message':
                user = hit['_source']['from']
                message = hit['_source']['message']
                if user == 'Miblo' and 'NOTE(annotator)' in message:
                    print(hit['_source']['recieved_at'], message)
                # Only parse questions after Casey triggers the Q&A
                if not process and user == 'abnercoimbre' or 'chronaldragon' or 'cmuratori' and '!qa' in message:
                    process = True
                if process and user != 'hmh_bot':
                    m = message.lower().strip()
                    words = m.split()
                    if (('q:' in m or '@cmuratori' in m or '@handmade_hero' in m) and
                        len(words) >= 4 and ('thanks' not in words or len >= 6)):
                        words_to_filter = ['q:', '@cmuratori', '@handmade_hero']
                        q = ' '.join([w for w in message.replace('"', '\\"').split() if w.lower() not in words_to_filter])
                        twitch_qs.append((user, q))

        twitch_video_time = r.json()['hits']['hits'][-1]['_source']['recieved_at'] # sic
        r = requests.get(url.format(twitch_video_id, twitch_video_time))

if not len(twitch_qs):
    print('No questions found on chat log')
    sys.exit(1)             ######################################## EXIT


print('Number of questions: {}'.format(len(twitch_qs)))

questions = []
qa_hour, qa_minute, qa_second = 1, 0, 0
if youtube_captions:
    # Process youtube captions
    f = StringIO.StringIO(youtube_captions)
    youtube_lines = []
    youtube_times = []
    while True:
        number_s = f.readline().strip()
        if not number_s:
            break

        number = int(number_s.strip())
        time = f.readline().strip()
        message = f.readline().strip()
        string = f.readline().strip()
        while string:
            message = ' '.join((message, string))
            string = f.readline().strip()
        youtube_lines.append(message)
        youtube_times.append(time)

    caption_words = []
    caption_times = []
    for sentence, time in zip(youtube_lines, youtube_times):
        word = []
        for c in sentence.lower():
            if c.isalpha():
                word.append(c)
            elif word:
                caption_words.append(''.join(word))
                caption_times.append(time)
                word = []

        # last word
        if word:
            caption_words.append(''.join(word))
            caption_times.append(time)

    # Look for time of Q&A

    # NOTE(matt): I arbitrarily changed the value of caption_words.index from
    # 'q' to 'r' in order to squash the error:
    #
    # Traceback (most recent call last):
    #  File "/home/matt/Software/Scripts/python/hmhstub.py", line 241, in <module>
    #      qa_index = caption_words.index('q')
    #      ValueError: 'q' is not in list

    qa_index = caption_words.index('q')
    if qa_index != -1:
        time = caption_times[qa_index]
        qa_hour, qa_minute, qa_second = [int(e.split(',')[0]) for e in time.split(':')[:3]]

    qa_time = qa_hour*3600 + qa_minute*60 + qa_second

    for user, q in twitch_qs:
        words = canonical(q)

        score, word_index = best_match(caption_words, words)
        if score > 0.3:
            #print 'Think I found one!'
            #print 'It goes like this:', caption_words[word_index:word_index+len(words)]
            time = caption_times[word_index]
            hour, minute, second = [int(e.split(',')[0]) for e in time.split(':')[:3]]
            q_time = hour*3600 + minute*60 + second
            if q_time > qa_time:
                questions.append((hour, minute, second, user, q))
            #print 'And it happens at: %02d:%02d:%02d'%(hour, minute, second)

questions = sorted(questions)

# Write basic template for video
with codecs.open('%s%03d.html.md'%(genre,int(episode_number)), 'w', encoding='utf-8') as f:
    f.write('---\n')
    f.write('title: "{}"\n'.format(title))
    f.write('videoId: "{}"\n'.format(youtube_video_id))
    f.write('markers:\n')
    f.write('    "%01d:%02d:%02d": "Q&A"\n'%(qa_hour, qa_minute, qa_second))
    if youtube_captions:
        for hour, minute, second, user, q in questions:
            f.write('    "%01d:%02d:%02d": "%s Q: %s"\n'%(hour, minute, second, user, q))

    f.write('---\n')

    f.write('\n    <!---\n')
    f.write('    HERE ARE ALL THE Qs, IN CASE YOU NEED TO COPY ANY OF THEM\n')
    for user, q in twitch_qs:
        f.write('    '+user+' Q: '+q+'\n')
    f.write('    -->\n')
