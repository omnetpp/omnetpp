// The right edges of the primary sidebar and of the content can be dragged to set their widths, and double-clicked
// to reset them. The widths are kept in the theme's storage, which is scoped to the site, and applied by
// templates/partials/javascripts/base.html.
(function () {
  const root = document.documentElement;

  // The page is centred and grows with the dragged part, so the edge moves slower than the width while there is
  // margin left, and as fast once the page fills the window: the width under the pointer is found by the secant
  // method.
  function follow(measure, apply, clamp, x) {
    let rect = measure();
    let width = clamp(rect.width + x - rect.right);
    for (let i = 0; i < 3; i++) {
      apply(width);
      const next = measure();
      const miss = x - next.right;
      const slope = (next.right - rect.right) / (next.width - rect.width);
      if (Math.abs(miss) < 1 || !(slope > 0)) break;
      rect = next;
      const target = clamp(next.width + miss / slope);
      if (target === width) break;
      width = target;
    }
  }

  function resizable({key, property, min, max, parent, seamClass, measure}) {
    const apply = width => {
      if (width) root.style.setProperty(property, width + 'px');
      else root.style.removeProperty(property);
    };
    const save = width => {
      try { __md_set(key, width); } catch (e) { }
    };
    const clamp = width => Math.round(Math.min(Math.max(width, min), max()));
    if (parent.querySelector(':scope > .omnetpp-seam')) return null;
    const seam = document.createElement('div');
    seam.className = 'omnetpp-seam ' + seamClass;
    seam.title = 'Drag to resize, double-click to reset';
    parent.appendChild(seam);
    seam.addEventListener('pointerdown', e => {
      if (e.button !== 0) return;
      e.preventDefault();
      seam.setPointerCapture(e.pointerId);
      root.classList.add('omnetpp-resizing');
      seam.classList.add('omnetpp-seam--active');
      let moved = false;
      const move = e => {
        moved = true;
        follow(measure, apply, clamp, e.clientX);
      };
      seam.addEventListener('pointermove', move);
      seam.addEventListener('lostpointercapture', () => {
        seam.removeEventListener('pointermove', move);
        root.classList.remove('omnetpp-resizing');
        seam.classList.remove('omnetpp-seam--active');
        if (moved) save(clamp(measure().width));
      }, {once: true});
    });
    seam.addEventListener('dblclick', () => {
      apply(null);
      save(null);
    });
    return seam;
  }

  function navWidth() {
    const probe = document.createElement('div');
    probe.style.cssText = 'position: absolute; visibility: hidden; width: var(--omnetpp-nav-width, 12.1rem)';
    document.body.appendChild(probe);
    const width = probe.offsetWidth;
    probe.remove();
    return width;
  }

  function attach() {
    const sidebar = document.querySelector('.md-sidebar--primary');
    if (sidebar) {
      const seam = resizable({
        key: 'omnetpp-nav.width', property: '--omnetpp-nav-width', min: 160, max: () => window.innerWidth / 2,
        parent: sidebar, seamClass: 'omnetpp-nav-seam', measure: () => sidebar.getBoundingClientRect(),
      });
      const wrap = sidebar.querySelector('.md-sidebar__scrollwrap');
      if (seam && wrap) {
        new ResizeObserver(() => {
          seam.style.top = wrap.offsetTop + 'px';
          seam.style.height = wrap.offsetHeight + 'px';
        }).observe(wrap);
      }
    }
    // The width is that of the content and the secondary sidebar together, so that it is the same on pages
    // without the secondary sidebar, where the content takes its place.
    const content = document.querySelector('.md-content');
    const inner = content && content.closest('.md-main__inner');
    if (content && inner) {
      // A hidden primary sidebar (e.g. of a tab navigated by its menu) leaves its width to the content. It does not
      // change while dragging, so it is measured once, at its start.
      let freed = 0;
      const seam = resizable({
        key: 'omnetpp-content.width', property: '--omnetpp-content-width', min: 480, max: () => window.innerWidth,
        parent: content, seamClass: 'omnetpp-content-seam',
        measure: () => {
          const rect = content.getBoundingClientRect();
          return {right: rect.right, width: inner.getBoundingClientRect().right - rect.left - freed};
        },
      });
      if (seam) {
        seam.addEventListener('pointerdown', () => {
          const sidebar = document.querySelector('.md-sidebar--primary');
          freed = sidebar && sidebar.hidden ? navWidth() : 0;
        });
      }
    }
  }

  if (window.document$) window.document$.subscribe(attach);
  else document.addEventListener('DOMContentLoaded', attach);
})();

// The theme's instant navigation makes every link of the page absolute, and the theme's tracking of the section in
// view then leaves the table of contents alone: it only follows links that start with "#". This follows the others,
// with the theme's classes, so that the theme's styling applies.
(function () {
  const OFFSET = 16;
  let controller = null;

  function entries(list) {
    const result = [];
    for (const a of list.querySelectorAll('a[href]')) {
      if (a.getAttribute('href').startsWith('#')) continue;
      const url = new URL(a.href);
      if (url.origin !== location.origin || url.pathname !== location.pathname || !url.hash) continue;
      const heading = document.getElementById(decodeURIComponent(url.hash.substring(1)));
      const link = a.closest('.md-nav__link');
      if (heading && link) result.push({link, heading});
    }
    return result;
  }

  function reveal(link) {
    const wrap = link.closest('.md-sidebar--secondary .md-sidebar__scrollwrap');
    if (!wrap || !link.offsetHeight) return;
    const top = link.getBoundingClientRect().top - wrap.getBoundingClientRect().top;
    if (top < 0 || top + link.offsetHeight > wrap.clientHeight) {
      wrap.scrollTo({top: wrap.scrollTop + top - wrap.clientHeight / 3, behavior: 'smooth'});
    }
  }

  function attach() {
    if (controller) controller.abort();
    controller = new AbortController();
    const lists = [...document.querySelectorAll('[data-md-component=toc]')].map(entries).filter(e => e.length);
    if (!lists.length) return;
    const header = document.querySelector('.md-header');
    const active = new Map();
    let pending = false;

    function update() {
      pending = false;
      // The tab bar, where it sticks under the header, covers what is under it as the header does.
      const tabs = document.querySelector('.md-tabs');
      const headerBottom = Math.max(header ? header.getBoundingClientRect().bottom : 0,
                                    tabs ? tabs.getBoundingClientRect().bottom : 0);
      // A link scrolls its heading, the target, to the heading's scroll margin, which the theme sets on the target
      // only: the heading is reached there.
      const target = document.querySelector(':target');
      const margin = target ? parseFloat(getComputedStyle(target).scrollMarginTop) || 0 : 0;
      const bottom = window.innerHeight + window.scrollY >= document.documentElement.scrollHeight - 1;
      lists.forEach((list, i) => {
        let current = -1;
        list.forEach((e, j) => {
          const limit = (e.heading === target ? Math.max(headerBottom, margin) : headerBottom) + OFFSET;
          if (bottom || e.heading.getBoundingClientRect().top <= limit) current = j;
        });
        list.forEach((e, j) => {
          e.link.classList.toggle('md-nav__link--passed', j <= current);
          e.link.classList.toggle('md-nav__link--active', j === current);
        });
        const link = current >= 0 ? list[current].link : null;
        if (link && link !== active.get(i)) reveal(link);
        active.set(i, link);
      });
    }

    function schedule() {
      if (pending) return;
      pending = true;
      requestAnimationFrame(update);
    }

    const options = {passive: true, signal: controller.signal};
    window.addEventListener('scroll', schedule, options);
    window.addEventListener('resize', schedule, options);
    window.addEventListener('hashchange', schedule, options);
    schedule();
  }

  if (window.document$) window.document$.subscribe(attach);
  else document.addEventListener('DOMContentLoaded', attach);
})();

// A page opened from the primary sidebar shows the sidebar as it was, with the clicked entry where it was clicked,
// instead of scrolled to where the theme (and a navigation that the site renders by script) put the entry of the page.
(function () {
  const KEY = 'omnetpp-nav.clicked';
  const TIMEOUT = 3000;
  const REVEAL = 1000;

  function page(url) {
    return url.split(/[?#]/)[0];
  }

  const root = document.documentElement;
  const HIDDEN = 'omnetpp-nav-restoring';

  document.addEventListener('click', e => {
    if (e.button !== 0 || e.ctrlKey || e.metaKey || e.shiftKey || e.altKey) return;
    const a = e.target.closest && e.target.closest('.md-sidebar--primary a.md-nav__link[href]');
    const wrap = a && a.closest('.md-sidebar__scrollwrap');
    if (!wrap || page(a.href) === page(location.href)) return;
    const state = {href: a.href, top: a.getBoundingClientRect().top, left: wrap.scrollLeft};
    try { sessionStorage.setItem(KEY, JSON.stringify(state)); } catch (err) { }
    // Hidden until restored, from now on, and on a new page from templates/partials/javascripts/base.html on.
    root.classList.add(HIDDEN);
    setTimeout(() => root.classList.remove(HIDDEN), TIMEOUT);
  }, true);

  // The entry may be rendered, or rendered again, later, and the sidebar may change above it: by a navigation that
  // the site renders by script, once it has loaded, which may then scroll to it too. The entry is kept in place
  // until then, or until the reader does anything.
  function restore() {
    let state = null;
    try {
      state = JSON.parse(sessionStorage.getItem(KEY));
      sessionStorage.removeItem(KEY);
    } catch (err) { }
    if (!state || page(state.href) !== page(location.href)) {
      root.classList.remove(HIDDEN);
      return;
    }
    // The entry of the current page links to "#", which is no hash.
    const hash = new URL(state.href).hash;
    const start = Date.now();
    let settled = 0;
    let last = null;
    let touched = false;
    const touch = () => { touched = true; };
    const events = ['wheel', 'pointerdown', 'touchstart', 'keydown'];
    for (const type of events) window.addEventListener(type, touch, {passive: true, once: true});
    (function attempt() {
      const wrap = document.querySelector('.md-sidebar--primary .md-sidebar__scrollwrap');
      const links = wrap ? [...wrap.querySelectorAll('a.md-nav__link[href]')] : [];
      const a = links.find(l => page(l.href) === page(state.href) && l.hash === hash);
      if (a && !touched) {
        const miss = a.getBoundingClientRect().top - state.top;
        if (Math.abs(miss) >= 1) wrap.scrollTop += miss;
        wrap.scrollLeft = state.left;
        // Where the sidebar cannot scroll far enough, the entry stops short of where it was clicked.
        const top = a.getBoundingClientRect().top;
        settled = last !== null && Math.abs(top - last) < 1 ? settled + 1 : 0;
        last = top;
      }
      // Shown once the entry has stopped moving, or without waiting for an entry that is not in the sidebar.
      if (touched || settled >= 2 || (!a && Date.now() - start >= REVEAL)) root.classList.remove(HIDDEN);
      if (!touched && Date.now() - start < TIMEOUT) setTimeout(attempt, 50);
      else {
        root.classList.remove(HIDDEN);
        for (const type of events) window.removeEventListener(type, touch);
      }
    })();
  }

  if (window.document$) window.document$.subscribe(restore);
  else document.addEventListener('DOMContentLoaded', restore);
})();

// A sidebar that scrolls sideways shows its horizontal scrollbar for a while (see omnetpp-immaterial.css).
(function () {
  const SHOWN = 1000;
  const lefts = new WeakMap();
  const timers = new WeakMap();

  document.addEventListener('scroll', e => {
    const wrap = e.target;
    if (!(wrap instanceof Element) || !wrap.classList.contains('md-sidebar__scrollwrap')) return;
    const left = lefts.get(wrap) || 0;
    lefts.set(wrap, wrap.scrollLeft);
    if (wrap.scrollLeft === left) return;
    wrap.classList.add('omnetpp-nav-scrolling');
    clearTimeout(timers.get(wrap));
    timers.set(wrap, setTimeout(() => wrap.classList.remove('omnetpp-nav-scrolling'), SHOWN));
  }, {capture: true, passive: true});
})();

// The theme pins the entries with children while their children scroll under them, each below the entries above
// it, but measures the entries only when the width of the page changes: on the first layout, which can be before
// that of the sidebar (all heights 0), and without the entries rendered later (e.g. by a navigation that the site
// renders by script). They are measured again, as the theme does, whenever the content of a sidebar changes its size.
(function () {
  let observer = null;

  function pin(root) {
    const pinned = new Map();
    for (const link of root.querySelectorAll('.md-nav__link')) {
      const nav = link.nextElementSibling;
      if (!(nav instanceof HTMLElement) || nav.tagName !== 'NAV') continue;
      const above = pinned.get(nav.parentElement.closest('nav'));
      const height = (above ? above.height + ' + ' : '') + `${link.offsetHeight}px + 0.625em`;
      const zindex = above ? above.zindex - 1 : 100;
      link.classList.add('md-nav__sticky');
      link.style.setProperty('--md-nav__sticky-zindex', String(zindex));
      nav.style.setProperty('--md-nav__header-height', `calc(${height})`);
      pinned.set(nav, {height, zindex});
    }
  }

  function attach() {
    if (observer) observer.disconnect();
    observer = new ResizeObserver(entries => {
      for (const entry of entries) {
        if (entry.target.querySelector('.md-nav__sticky')) pin(entry.target);
      }
    });
    for (const inner of document.querySelectorAll('.md-sidebar .md-sidebar__inner')) observer.observe(inner);
  }

  if (window.document$) window.document$.subscribe(attach);
  else document.addEventListener('DOMContentLoaded', attach);
})();

// The name of the site in the header leads where the logo next to it does.
(function () {
  function attach() {
    const logo = document.querySelector('.md-header__button.md-logo');
    const name = document.querySelector('.md-header__title .md-header__topic:not([data-md-component]) .md-ellipsis');
    if (!logo || !name || name.querySelector('a')) return;
    const a = document.createElement('a');
    a.href = logo.href;
    a.className = 'omnetpp-header-home';
    a.append(...name.childNodes);
    name.append(a);
  }

  if (window.document$) window.document$.subscribe(attach);
  else document.addEventListener('DOMContentLoaded', attach);
})();

// Since Sphinx 8.2, language_data.js (loaded by the search on demand) defines the stop words as a Set, and exports it
// as window.stopwords, but the search of the theme still looks them up as in an array.
(function () {
  let stopwords;
  Object.defineProperty(window, 'stopwords', {
    configurable: true,
    get: () => stopwords,
    set: (value) => {
      if (value instanceof Set && !value.indexOf)
        value.indexOf = (word) => value.has(word) ? 0 : -1;
      stopwords = value;
    },
  });
})();

// The entries of the primary sidebar that lead into the parts of the site named by omnetpp_nav_nowrap (parts of
// URL paths), and the secondary sidebar on their pages, do not wrap (see omnetpp-immaterial.css). The entries may
// be rendered later, e.g. by a navigation that the site renders by script, or when a list is first expanded.
(function () {
  const parts = window.omnetppNavNowrap || [];
  if (!parts.length) return;
  let observer = null;

  function nowrap(url) {
    const path = new URL(url, location.href).pathname;
    return parts.some(part => path.includes(part));
  }

  function mark(root) {
    for (const a of root.querySelectorAll('a.md-nav__link[href]')) a.classList.toggle('omnetpp-nowrap', nowrap(a.href));
  }

  function attach() {
    const secondary = document.querySelector('.md-sidebar--secondary');
    if (secondary) secondary.classList.toggle('omnetpp-nowrap', nowrap(location.href));
    const primary = document.querySelector('.md-sidebar--primary');
    if (!primary) return;
    mark(primary);
    if (observer) observer.disconnect();
    observer = new MutationObserver(records => {
      for (const record of records) {
        for (const node of record.addedNodes) if (node instanceof Element) mark(node);
      }
    });
    observer.observe(primary, {childList: true, subtree: true});
  }

  if (window.document$) window.document$.subscribe(attach);
  else document.addEventListener('DOMContentLoaded', attach);
})();

// The tabs open their sections as menus, like those of an operating system: a tab opens the entries of its section
// while the pointer is over it, and an entry with entries of its own opens them in a submenu beside it. The entries
// are those of the navigation that the page carries (the primary sidebar, also where it is hidden), so a section
// that the page is not in shows only its first level. Clicks still follow the links of the tabs and the entries.
// Touch screens, which have no hover, get no menus, and neither do narrow screens, which have no tabs.
(function () {
  const options = window.omnetppNavMenus;
  if (!options) return;
  const OPEN_DELAY = 150;
  const CLOSE_DELAY = 300;
  const SUBMENU_DELAY = 100;
  const MARGIN = 8;
  const wide = window.matchMedia('(min-width: 76.25em)');
  const entries = new WeakMap();
  const menus = [];
  let tab = null;
  let openTimer = null;
  let closeTimer = null;
  let submenuTimer = null;
  let returning = false;

  function own(li) {
    return li.querySelector(':scope > a.md-nav__link') ||
      li.querySelector(':scope > .md-nav__container > a.md-nav__link') ||
      li.querySelector(':scope > label.md-nav__link');
  }

  // The page sections of the current page (its table of contents) are not entries: neither in its own entry, nor in
  // the second one that the theme adds for them where the page has entries of its own (md-nav__current-toc). An
  // entry marked as a leaf (data-omnetpp-menu="leaf", e.g. a list too long for a menu) has no submenu.
  function children(li, depth) {
    const list = li.querySelector(':scope > nav.md-nav:not(.md-nav--secondary) > .md-nav__list');
    if (!list || depth < 1 || li.dataset.omnetppMenu === 'leaf') return [];
    return [...list.children]
      .filter(child => !child.classList.contains('md-nav__current-toc'))
      .map(child => entry(child, depth))
      .filter(Boolean);
  }

  function entry(li, depth) {
    const link = own(li);
    if (!link) return null;
    const title = (link.querySelector('.md-ellipsis') || link).textContent.trim();
    if (!title) return null;
    return {
      title,
      href: link instanceof HTMLAnchorElement ? link.href.replace(/#$/, '') : null,
      active: li.classList.contains('md-nav__item--active'),
      children: children(li, depth - 1),
    };
  }

  function section(tabItem) {
    const title = tabItem.textContent.trim();
    const items = document.querySelectorAll('.md-sidebar--primary .md-nav--primary > .md-nav__list > li');
    const li = [...items].find(item => {
      const link = own(item);
      return link && (link.querySelector('.md-ellipsis') || link).textContent.trim() === title;
    });
    return li ? children(li, options.depth) : [];
  }

  function levelOf(element) {
    return menus.indexOf(element.closest('.omnetpp-menu'));
  }

  function items(menu) {
    return [...menu.querySelectorAll(':scope > .omnetpp-menu__item')];
  }

  function render(list) {
    const menu = document.createElement('div');
    menu.className = 'omnetpp-menu';
    menu.setAttribute('role', 'menu');
    for (const e of list) {
      const item = document.createElement(e.href ? 'a' : 'span');
      item.className = 'omnetpp-menu__item';
      if (e.active) item.classList.add('omnetpp-menu__item--active');
      if (e.children.length) item.classList.add('omnetpp-menu__item--parent');
      if (e.href) item.href = e.href;
      item.tabIndex = -1;
      item.title = e.title;
      item.setAttribute('role', 'menuitem');
      const label = document.createElement('span');
      label.className = 'omnetpp-menu__label';
      label.textContent = e.title;
      item.appendChild(label);
      entries.set(item, e);
      menu.appendChild(item);
    }
    menu.addEventListener('pointerenter', e => { if (e.pointerType !== 'touch') clearTimeout(closeTimer); });
    menu.addEventListener('pointerleave', e => { if (e.pointerType !== 'touch') scheduleClose(); });
    menu.addEventListener('pointerover', e => {
      const item = e.target.closest('.omnetpp-menu__item');
      if (!item || e.pointerType === 'touch') return;
      clearTimeout(submenuTimer);
      submenuTimer = setTimeout(() => openSubmenu(item, false), SUBMENU_DELAY);
    });
    menu.addEventListener('keydown', keydown);
    // An entry without a page of its own opens its submenu; the others are links, which close the menus.
    menu.addEventListener('click', e => {
      const item = e.target.closest('.omnetpp-menu__item');
      if (item && item.tagName !== 'A') openSubmenu(item, false);
      else setTimeout(closeAll);
    });
    return menu;
  }

  // Below the tab bar, or beside the entry of a submenu, on the other side where it would leave the window, and
  // scrolling where it is taller than the window.
  function place(menu, anchor, level) {
    document.body.appendChild(menu);
    const width = menu.offsetWidth;
    const right = document.documentElement.clientWidth - MARGIN;
    let left, top;
    if (level === 0) {
      const tabs = anchor.closest('.md-tabs');
      left = anchor.getBoundingClientRect().left;
      top = (tabs || anchor).getBoundingClientRect().bottom;
      if (left + width > right) left = anchor.getBoundingClientRect().right - width;
    } else {
      const rect = anchor.getBoundingClientRect();
      left = rect.right;
      top = rect.top - parseFloat(getComputedStyle(menu).paddingTop);
      if (left + width > right) left = rect.left - width;
    }
    top = Math.max(MARGIN, Math.min(top, window.innerHeight - MARGIN - menu.offsetHeight));
    menu.style.left = Math.max(MARGIN, left) + 'px';
    menu.style.top = top + 'px';
    menu.style.maxHeight = (window.innerHeight - MARGIN - top) + 'px';
  }

  function closeFrom(level) {
    while (menus.length > level) menus.pop().remove();
    const parent = menus[level - 1];
    if (parent) for (const item of items(parent)) item.classList.remove('omnetpp-menu__item--open');
  }

  function closeAll() {
    clearTimeout(openTimer);
    clearTimeout(closeTimer);
    clearTimeout(submenuTimer);
    closeFrom(0);
    if (tab) tab.classList.remove('omnetpp-tab--open');
    tab = null;
  }

  function scheduleClose() {
    clearTimeout(closeTimer);
    closeTimer = setTimeout(closeAll, CLOSE_DELAY);
  }

  function focus(menu, index) {
    const list = items(menu);
    if (list.length) list[(index + list.length) % list.length].focus();
  }

  function openMenu(tabItem, focusFirst) {
    clearTimeout(openTimer);
    clearTimeout(closeTimer);
    if (!wide.matches) return;
    if (tab !== tabItem || !menus.length) {
      closeAll();
      const list = section(tabItem);
      if (!list.length) return;
      tab = tabItem;
      tab.classList.add('omnetpp-tab--open');
      const menu = render(list);
      menus.push(menu);
      place(menu, tabItem, 0);
    }
    if (focusFirst) focus(menus[0], 0);
  }

  function openSubmenu(item, focusFirst) {
    const level = levelOf(item);
    if (level < 0) return;
    const e = entries.get(item);
    if (item.classList.contains('omnetpp-menu__item--open') && menus.length > level + 1) {
      if (focusFirst) focus(menus[level + 1], 0);
      return;
    }
    closeFrom(level + 1);
    if (!e || !e.children.length) return;
    item.classList.add('omnetpp-menu__item--open');
    const menu = render(e.children);
    menus.push(menu);
    place(menu, item, level + 1);
    if (focusFirst) focus(menu, 0);
  }

  // The focus goes back to the tab without opening its menu again, also for Tab to move on from there rather than
  // from the menu at the end of the page.
  function returnToTab() {
    const link = tab && tab.querySelector('.md-tabs__link');
    closeAll();
    if (!link) return;
    returning = true;
    link.focus();
    returning = false;
  }

  function back(level) {
    const parent = menus[level - 1];
    const item = parent.querySelector(':scope > .omnetpp-menu__item--open') || items(parent)[0];
    closeFrom(level);
    if (item) item.focus();
  }

  function keydown(e) {
    const item = e.target.closest('.omnetpp-menu__item');
    if (!item) return;
    const level = levelOf(item);
    const list = items(menus[level]);
    const index = list.indexOf(item);
    switch (e.key) {
      case 'ArrowDown': focus(menus[level], index + 1); break;
      case 'ArrowUp': focus(menus[level], index - 1); break;
      case 'Home': focus(menus[level], 0); break;
      case 'End': focus(menus[level], -1); break;
      case 'ArrowRight': openSubmenu(item, true); break;
      case 'Enter':
        if (item.tagName === 'A') return;
        openSubmenu(item, true);
        break;
      case 'ArrowLeft':
        if (level > 0) back(level);
        break;
      case 'Escape':
        if (level > 0) back(level);
        else returnToTab();
        break;
      case 'Tab': returnToTab(); return;
      default: return;
    }
    e.preventDefault();
  }

  function attach() {
    closeAll();
    for (const tabItem of document.querySelectorAll('.md-tabs__item')) {
      if (tabItem.dataset.omnetppMenu) continue;
      tabItem.dataset.omnetppMenu = 'attached';
      tabItem.addEventListener('pointerenter', e => {
        if (e.pointerType === 'touch') return;
        clearTimeout(closeTimer);
        clearTimeout(openTimer);
        if (menus.length) openMenu(tabItem, false);
        else openTimer = setTimeout(() => openMenu(tabItem, false), OPEN_DELAY);
      });
      tabItem.addEventListener('pointerleave', e => {
        if (e.pointerType === 'touch') return;
        clearTimeout(openTimer);
        scheduleClose();
      });
      const link = tabItem.querySelector('.md-tabs__link');
      if (!link) continue;
      link.addEventListener('focus', () => {
        if (!returning && link.matches(':focus-visible')) openMenu(tabItem, false);
      });
      link.addEventListener('keydown', e => {
        if (e.key === 'ArrowDown') {
          e.preventDefault();
          openMenu(tabItem, true);
        } else if (e.key === 'Escape') closeAll();
      });
    }
  }

  // Focus that leaves the tabs and the menus, e.g. by a click elsewhere, closes them.
  document.addEventListener('focusin', e => {
    if (!menus.length || (e.target.closest && e.target.closest('.omnetpp-menu')) || (tab && tab.contains(e.target))) return;
    closeAll();
  });
  document.addEventListener('pointerdown', e => {
    if (menus.length && !(e.target.closest && e.target.closest('.omnetpp-menu, .md-tabs__item'))) closeAll();
  });
  window.addEventListener('scroll', () => { if (menus.length) closeAll(); }, {passive: true});
  window.addEventListener('resize', () => { if (menus.length) closeAll(); });
  wide.addEventListener('change', closeAll);

  if (window.document$) window.document$.subscribe(attach);
  else document.addEventListener('DOMContentLoaded', attach);
})();

// The tab bar comes back under the header as soon as the reader scrolls up, as the theme's back-to-top button does,
// and goes again when the reader scrolls down (see omnetpp-immaterial.css); the theme shows it only at the top of
// the page. A page opens with the tab bar shown, also scrolled down (e.g. reloaded, or opened at a section), and
// keeps it until the reader scrolls: scrolling that the browser or the theme does does not count. Neither do small
// movements, e.g. of a touchpad coming to rest.
(function () {
  const THRESHOLD = 4;
  const INPUT = ['wheel', 'keydown', 'touchstart', 'pointerdown'];
  const root = document.documentElement;
  let active = false;
  let last = window.scrollY;

  function activate() {
    active = true;
    for (const type of INPUT) window.removeEventListener(type, activate, true);
  }

  // The position is followed also before the reader scrolls, so that the first scrolling of the reader counts from
  // where it started, whether its input event comes before or after the scrolling itself.
  window.addEventListener('scroll', () => {
    const y = window.scrollY;
    if (!active) {
      last = y;
      return;
    }
    if (y <= 0) root.classList.remove('omnetpp-tabs-away');
    if (Math.abs(y - last) < THRESHOLD) return;
    root.classList.toggle('omnetpp-tabs-away', y > last && y > 0);
    last = y;
  }, {passive: true});

  // What goes below the tab bar while it is shown (omnetpp-immaterial.css) needs its height, which a site may change
  // (e.g. by a border).
  function measure() {
    const tabs = document.querySelector('.md-tabs');
    if (tabs && tabs.offsetHeight) root.style.setProperty('--omnetpp-tabs-height', tabs.offsetHeight + 'px');
  }

  function reset() {
    active = false;
    last = window.scrollY;
    root.classList.remove('omnetpp-tabs-away');
    for (const type of INPUT) window.addEventListener(type, activate, {capture: true, passive: true});
    measure();
  }

  window.addEventListener('resize', measure, {passive: true});
  reset();
  if (window.document$) window.document$.subscribe(reset);
})();
