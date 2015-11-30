#!/bin/bash
#
#	Script to get timestamp from mpv into vim + set mpv timestamp from the text
#	in the current line of vim. By insofaras.
#
#	Requires: socat, date, grep, sed
#
#	Add this to your vimrc, replacing the /path/ and the <???> key mappings
#
#	function! MPVSetTime()
# 	  let l = shellescape(getline('.'))
#	  call system("/path/to/this-script.sh " . l)
#	endfunction
#
#	nnoremap <???> :call MPVSetTime()<CR>
#	nnoremap <???> :r!/path/to/this-script.sh<CR>t"
#
#	And start mpv with:
#		$ mpv --input-unix-socket=/tmp/mpvsocket hmh-video.mp4

socket=/tmp/mpvsocket

get_time() {
	get_cmd='{ "command": ["get_property", "time-pos"] }'
	mpvdata=$(socat - "$socket" <<< "$get_cmd")

	ts_raw=$(sed -rn 's/.*"data":([0-9]+)\..*$/\1/p' <<< "$mpvdata")
	ts_hours=$(date -d@"$ts_raw" -u +%-H)

	if [[ $ts_hours != 0 ]]; then
		timestamp=$(date -d@"$ts_raw" -u +%-H:%M:%S)
	else
		timestamp=$(date -d@"$ts_raw" -u +%-M:%S)
	fi

	echo "    \"$timestamp\": \"\""
}

set_time() {
	ts_raw=$(sed -rn 's/^[^\"]+\"([0-9:]+)\".*$/\1/p' <<< "$1")
	
	num_colons=$(grep -o : <<<"$ts_raw" | wc -l)

	if [[ "$num_colons" == 1 ]]; then
		ts_raw="0:$ts_raw"
	fi

	timestamp=$(date -d "Jan 01 1970 $ts_raw" -u +%s)
	set_cmd="{ \"command\": [\"set_property\", \"time-pos\", $timestamp] }"
	socat - "$socket" <<< "$set_cmd" >/dev/null 2>&1
}

if [[ -n "$1" ]]; then
	set_time "$1"
else
	get_time
fi


