function! RunDebug()
    silent !ctags src/*.c src/shared/*.h
    silent !cmake -D CMAKE_BUILD_TYPE:STRING=Debug .
    !make && ./paysages
endfunction

function! RunRelease()
    silent !ctags src/*.c src/shared/*.h
    silent !cmake -D CMAKE_BUILD_TYPE:STRING=Release .
    !make && ./paysages
endfunction

function! RunProfile()
    silent !ctags src/*.c src/shared/*.h
    silent !cmake -D CMAKE_BUILD_TYPE:STRING=Profile .
    !make && ./paysages && gprof
endfunction

map <silent> <F9> :call RunDebug()<CR>
map <silent> <C-F9> :call RunProfile()<CR>
map <silent> <A-F9> :call RunRelease()<CR>
map <silent> <F10> :!eog output/resultaa.png<CR>

