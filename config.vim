function! UpdateTags()
    !ctags --recurse=yes ./src/
endfunction

function! RunDebug()
    UpdateTags()
    !make BUILDMODE=debug all run_qt
endfunction

function! RunRelease()
    UpdateTags()
    !make BUILDMODE=debug all run_qt
endfunction

map <silent> <S-F9> :call RunDebug()<CR>
map <silent> <S-F10> :call RunRelease()<CR>
map <silent> <F11> :!make tests<CR>

