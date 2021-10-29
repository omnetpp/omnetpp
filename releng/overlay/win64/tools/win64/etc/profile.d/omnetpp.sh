##############################################################
# OMNeT++ specific changes

export TERM=xterm-256color

# source the git completion script if exsist
if [ -f "/usr/share/bash-completion/completions/git" ] ; then
  source "/usr/share/bash-completion/completions/git"
fi
if [ -f "/usr/share/git/git-prompt.sh" ] ; then
  source "/usr/share/git/git-prompt.sh"
fi

export PS1='\[\e]0;\w\a\]\n\[\e[32m\]\[\e[33m\]$(__git_ps1 "(%s) ")\w\[\e[0m\]\$ '

if [ -f "$HOME/setenv" ] ; then
  source $HOME/setenv
fi
